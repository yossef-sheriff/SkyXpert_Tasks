/*
 * UART.c
 */
#include "UART.h"
#include "GPIO.h"
#include "EXTI.h"          // NVIC_SET_ENABLE

#define RX_BUF_SIZE		32		// must be a power of 2
#define RX_BUF_MASK		(RX_BUF_SIZE - 1)

static volatile uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint8_t rx_head = 0;   // written by ISR
static volatile uint8_t rx_tail = 0;   // written by main

void MCAL_UART1_Init(void)
{
	GPIO_PinConfig_t cfg;

	RCC_GPIO_A_CLK_EN();
	RCC_USART1_CLK_EN();

	// PA9 (TX): alternate function push-pull, 50 MHz
	cfg.GPIO_PinNumber    = GPIO_PIN_9;
	cfg.GPIO_MODE         = GPIO_MODE_OUTPUT_AF_PP;
	cfg.GPIO_Output_Speed = GPIO_SPEED_50M;
	MCAL_GPIO_Init(GPIO_A, &cfg);

	// PA10 (RX): input floating
	cfg.GPIO_PinNumber    = GPIO_PIN_10;
	cfg.GPIO_MODE         = GPIO_MODE_INPUT_FLO;
	cfg.GPIO_Output_Speed = GPIO_SPEED_2M;
	MCAL_GPIO_Init(GPIO_A, &cfg);

	USART1->BRR = USART_BRR_VALUE(PCLK2_HZ, UART_BAUD);

	// UE | TE | RE | RXNEIE   -> 8 data bits, no parity, 1 stop bit (defaults)
	USART1->CR1 = (1<<13) | (1<<3) | (1<<2) | (1<<5);
	USART1->CR2 = 0;
	USART1->CR3 = 0;

	NVIC_SET_ENABLE->NVIC_ISER1 = (1<<5);   // USART1 IRQ = 37 -> ISER1 bit 5
}

void MCAL_UART1_SendByte(uint8_t byte)
{
	while (!(USART1->SR & (1<<7)));   // wait TXE
	USART1->DR = byte;
}

uint8_t MCAL_UART1_Available(void)
{
	return (rx_head != rx_tail) ? 1 : 0;
}

uint8_t MCAL_UART1_ReadByte(void)
{
	uint8_t byte = rx_buf[rx_tail];
	rx_tail = (rx_tail + 1) & RX_BUF_MASK;
	return byte;
}

void USART1_IRQHandler(void)
{
	uint32_t sr = USART1->SR;

	if (sr & ((1<<5) | (1<<3)))          // RXNE or ORE
	{
		uint8_t byte = (uint8_t)USART1->DR;   // reading DR clears RXNE/ORE

		if (sr & (1<<5))                  // only store if the byte is valid
		{
			uint8_t next = (rx_head + 1) & RX_BUF_MASK;
			if (next != rx_tail)          // drop byte if buffer is full
			{
				rx_buf[rx_head] = byte;
				rx_head = next;
			}
		}
	}
}
