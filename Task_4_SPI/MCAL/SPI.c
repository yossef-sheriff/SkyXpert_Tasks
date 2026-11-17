/*
 * SPI.c
 */
#include "SPI.h"
#include "GPIO.h"
#include "EXTI.h"          // NVIC_SET_ENABLE / NVIC_CLEAR_ENABLE

#define SPI1_IRQ_BIT		(1UL << 3)     // SPI1 IRQ = 35 -> NVIC_ISER1 bit 3
#define SPI_TIMEOUT_LOOPS	20000UL

static void (*spi1_callback)(uint8_t rx) = NULL;

void MCAL_SPI1_Init(SPI_Config_t* SPI_Config)
{
	GPIO_PinConfig_t pin;
	uint32_t cr1 = 0;
	uint32_t cr2 = 0;

	RCC_GPIO_A_CLK_EN();
	RCC_SPI1_CLK_EN();

	pin.GPIO_Output_Speed = GPIO_SPEED_50M;

	if (SPI_Config->Mode == SPI_MODE_MASTER)
	{
		// PA5 = SCK, PA7 = MOSI: alternate function push-pull
		pin.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
		pin.GPIO_PinNumber = GPIO_PIN_5;
		MCAL_GPIO_Init(GPIO_A, &pin);
		pin.GPIO_PinNumber = GPIO_PIN_7;
		MCAL_GPIO_Init(GPIO_A, &pin);

		// PA6 = MISO: input floating
		pin.GPIO_MODE = GPIO_MODE_INPUT_FLO;
		pin.GPIO_PinNumber = GPIO_PIN_6;
		MCAL_GPIO_Init(GPIO_A, &pin);

		if (SPI_Config->NSS == SPI_NSS_HW_OUTPUT)
		{
			// PA4 = NSS output (SSOE)
			pin.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			pin.GPIO_PinNumber = GPIO_PIN_4;
			MCAL_GPIO_Init(GPIO_A, &pin);
			cr2 |= (1<<2);                         // SSOE
		}

		cr1 |= (1<<2);                             // MSTR
		cr1 |= ((uint32_t)(SPI_Config->Baud & 0x07) << 3);
	}
	else
	{
		// PA5 = SCK, PA7 = MOSI: input floating
		pin.GPIO_MODE = GPIO_MODE_INPUT_FLO;
		pin.GPIO_PinNumber = GPIO_PIN_5;
		MCAL_GPIO_Init(GPIO_A, &pin);
		pin.GPIO_PinNumber = GPIO_PIN_7;
		MCAL_GPIO_Init(GPIO_A, &pin);

		// PA6 = MISO: alternate function push-pull
		pin.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
		pin.GPIO_PinNumber = GPIO_PIN_6;
		MCAL_GPIO_Init(GPIO_A, &pin);

		if (SPI_Config->NSS == SPI_NSS_HW_INPUT)
		{
			// PA4 = NSS input
			pin.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			pin.GPIO_PinNumber = GPIO_PIN_4;
			MCAL_GPIO_Init(GPIO_A, &pin);
		}
	}

	// Clock polarity and phase
	cr1 |= ((uint32_t)(SPI_Config->CPHA & 0x01) << 0);
	cr1 |= ((uint32_t)(SPI_Config->CPOL & 0x01) << 1);

	// NSS management
	if (SPI_Config->NSS == SPI_NSS_SOFT)
	{
		cr1 |= (1<<9);                             // SSM
		if (SPI_Config->Mode == SPI_MODE_MASTER)
		{
			cr1 |= (1<<8);                         // SSI = 1 (avoid MODF)
		}
	}
	// SPI_NSS_HW_INPUT / HW_OUTPUT: SSM = 0 (default)

	// 8-bit data, MSB first (DFF = 0, LSBFIRST = 0) - defaults after reset

	// Interrupt
	if (SPI_Config->IRQ_EN == SPI_IRQ_ENABLE)
	{
		cr2 |= (1<<6);                             // RXNEIE
		spi1_callback = SPI_Config->P_IRQ_Callback;
		NVIC_SET_ENABLE->NVIC_ISER1 = SPI1_IRQ_BIT;
	}
	else
	{
		spi1_callback = NULL;
		NVIC_CLEAR_ENABLE->NVIC_ICER1 = SPI1_IRQ_BIT;
	}

	// Configure everything first, then enable the peripheral (SPE last)
	SPI1->CR1 = 0;
	SPI1->CR2 = cr2;
	SPI1->CR1 = cr1 | (1<<6);                      // SPE
}

void MCAL_SPI1_DeInit(void)
{
	RCC->RCC_APB2RSTR |= (1<<12);                  // SPI1 reset
	RCC->RCC_APB2RSTR &= ~(1<<12);

	NVIC_CLEAR_ENABLE->NVIC_ICER1 = SPI1_IRQ_BIT;
	spi1_callback = NULL;
}

uint8_t MCAL_SPI1_TransferByte(uint8_t data)
{
	uint32_t t = SPI_TIMEOUT_LOOPS;

	while (!(SPI1->SR & (1<<1)))                   // TXE
	{
		if (--t == 0) return 0xFF;
	}
	SPI1->DR = data;

	t = SPI_TIMEOUT_LOOPS;
	while (!(SPI1->SR & (1<<0)))                   // RXNE
	{
		if (--t == 0) return 0xFF;
	}
	return (uint8_t)SPI1->DR;
}

void MCAL_SPI1_SetTxData(uint8_t data)
{
	// Non-blocking. Writing DR while TX buffer is full overwrites it,
	// so the slave must call this once per received byte.
	SPI1->DR = data;
}

uint8_t MCAL_SPI1_IsBusy(void)
{
	return (SPI1->SR & (1<<7)) ? 1 : 0;            // BSY
}

void SPI1_IRQHandler(void)
{
	uint32_t sr = SPI1->SR;

	// Overrun: clear by reading DR then SR
	if (sr & (1<<6))                               // OVR
	{
		volatile uint32_t tmp = SPI1->DR;
		tmp = SPI1->SR;
		(void)tmp;
		return;
	}

	if (sr & (1<<0))                               // RXNE
	{
		uint8_t rx = (uint8_t)SPI1->DR;            // reading DR clears RXNE
		if (spi1_callback != NULL)
		{
			spi1_callback(rx);
		}
	}
}
