/*
 * main_master.c
 *
 * MCU_1 (Master / Display unit)
 *   LCD  : 16x2 via PCF8574 on I2C1 (PB6 SCL, PB7 SDA)
 *   UART : PA10 = RX <- slave TX (PA9)
 */
#include "config.h"

#if defined(MCU_ROLE_MASTER)

#include "stm32f103x8.h"
#include "TIMER.h"
#include "UART.h"
#include "LCD.h"

#define EVENT_DURATION_MS		2000UL

static uint8_t  led_state    = PROTO_STATE_BLINKING;   // default after reset
static uint8_t  event_active = 0;
static uint32_t event_start  = 0;
static uint8_t  display_dirty = 1;

static void Handle_Byte(uint8_t byte)
{
	if (byte == PROTO_EVENT_INTERRUPT)
	{
		event_active  = 1;
		event_start   = MCAL_TIM2_GetMs();   // restart the 2 s window on each event
		display_dirty = 1;
	}
	else if (byte == PROTO_STATE_BLINKING || byte == PROTO_STATE_HIGH)
	{
		// Stored even during the event window; shown once the event ends
		led_state     = byte;
		display_dirty = 1;
	}
	// Unknown bytes are ignored
}

static void Update_Event(void)
{
	if (event_active && MCAL_TIM2_Elapsed(event_start, EVENT_DURATION_MS))
	{
		event_active  = 0;
		display_dirty = 1;
	}
}

// Writes exactly 16 characters per line so no old characters remain (no LCD_Clear flicker)
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
		// "Interrupt happened" is 18 chars, so it is split over the two lines
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
	MCAL_TIM2_Init();
	MCAL_UART1_Init();
	LCD_Init();

	while (1)
	{
		while (MCAL_UART1_Available())
		{
			Handle_Byte(MCAL_UART1_ReadByte());
		}

		Update_Event();

		if (display_dirty)
		{
			display_dirty = 0;
			Refresh_Display();
		}
	}
}

#endif /* MCU_ROLE_MASTER */
