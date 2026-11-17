/*
 * main_master.c  -  SPI version
 *
 *   LCD  : 16x2 via PCF8574 on I2C1 (PB6 SCL, PB7 SDA)
 *   SPI1 : PA4 = CS (software, GPIO output), PA5 SCK, PA6 MISO, PA7 MOSI
 */
#include "config.h"

#if defined(MCU_ROLE_MASTER) && defined(COMM_MODE_SPI)

#include "stm32f103x8.h"
#include "GPIO.h"
#include "TIMER.h"
#include "SPI.h"
#include "I2C.h"
#include "LCD.h"

#define CS_PORT				GPIO_A
#define CS_PIN				GPIO_PIN_4

#define POLL_PERIOD_MS		100UL
#define EVENT_DURATION_MS	2000UL

static uint8_t  led_state     = PROTO_STATE_BLINKING;
static uint8_t  event_active  = 0;
static uint32_t event_start   = 0;
static uint8_t  display_dirty = 1;

// Gives the slave time to preload its next reply byte
static void Inter_Byte_Gap(void)
{
	for (volatile uint32_t i = 0; i < 100; i++);
}

static uint8_t Spi_Transfer(uint8_t data)
{
	uint8_t rx = MCAL_SPI1_TransferByte(data);
	Inter_Byte_Gap();
	return rx;
}

// One poll = one 3-byte frame
static void Spi_Poll(void)
{
	uint8_t r0, r1, r2;

	MCAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

	r0 = Spi_Transfer(PROTO_SPI_CMD_POLL);
	r1 = Spi_Transfer(0x00);
	r2 = Spi_Transfer(0x00);

	MCAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

	if (r0 != PROTO_SPI_HEADER)
	{
		return;                                  // invalid frame, ignore
	}

	if (r1 == PROTO_EVENT_INTERRUPT)
	{
		event_active  = 1;
		event_start   = MCAL_TIM2_GetMs();
		display_dirty = 1;
	}

	if ((r2 == PROTO_STATE_BLINKING || r2 == PROTO_STATE_HIGH) && r2 != led_state)
	{
		led_state     = r2;
		display_dirty = 1;
	}
}

static void Update_Event(void)
{
	if (event_active && MCAL_TIM2_Elapsed(event_start, EVENT_DURATION_MS))
	{
		event_active  = 0;
		display_dirty = 1;
	}
}

static void LCD_ShowLine(uint8_t row, const char *text)
{
	char buf[17];
	uint8_t i = 0;

	while (i < 16 && text[i])
	{
		buf[i] = text[i];
		i++;
	}
	while (i < 16)
	{
		buf[i++] = ' ';
	}
	buf[16] = '\0';

	LCD_SetCursor(row, 0);
	LCD_PrintString(buf);
}

static void Refresh_Display(void)
{
	if (event_active)
	{
		LCD_ShowLine(0, "Interrupt");
		LCD_ShowLine(1, "happened");
	}
	else if (led_state == PROTO_STATE_HIGH)
	{
		LCD_ShowLine(0, "LED is HIGH");
		LCD_ShowLine(1, "");
	}
	else
	{
		LCD_ShowLine(0, "LED is Blinking");
		LCD_ShowLine(1, "");
	}
}

int main(void)
{
	GPIO_PinConfig_t cs;
	SPI_Config_t spi;
	uint32_t last_poll;

	MCAL_TIM2_Init();
	MCAL_I2C1_Init();
	LCD_Init();

	// CS on PA4 as GPIO output, idle high
	RCC_GPIO_A_CLK_EN();
	cs.GPIO_PinNumber    = CS_PIN;
	cs.GPIO_MODE         = GPIO_MODE_OUTPUT_PP;
	cs.GPIO_Output_Speed = GPIO_SPEED_2M;
	MCAL_GPIO_Init(CS_PORT, &cs);
	MCAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

	spi.Mode           = SPI_MODE_MASTER;
	spi.Baud           = SPI_BAUD_DIV16;     // 8 MHz / 16 = 500 kHz
	spi.CPOL           = SPI_CPOL_LOW;
	spi.CPHA           = SPI_CPHA_FIRST;
	spi.NSS            = SPI_NSS_SOFT;
	spi.IRQ_EN         = SPI_IRQ_DISABLE;
	spi.P_IRQ_Callback = 0;
	MCAL_SPI1_Init(&spi);

	last_poll = MCAL_TIM2_GetMs() - POLL_PERIOD_MS;   // poll immediately

	while (1)
	{
		if (MCAL_TIM2_Elapsed(last_poll, POLL_PERIOD_MS))
		{
			last_poll = MCAL_TIM2_GetMs();
			Spi_Poll();
		}

		Update_Event();

		if (display_dirty)
		{
			display_dirty = 0;
			Refresh_Display();
		}
	}
}

#endif /* MCU_ROLE_MASTER && COMM_MODE_SPI */
