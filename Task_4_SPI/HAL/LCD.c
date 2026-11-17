/*
 * LCD.c
 */
#include "LCD.h"
#include "I2C.h"
#include "TIMER.h"
#include "config.h"

#define LCD_RS		(1<<0)
#define LCD_RW		(1<<1)     // always 0 (write mode)
#define LCD_EN		(1<<2)
#define LCD_BL		(1<<3)     // backlight

// Approximate busy-wait. At 8 MHz it is always >= the requested time.
static void delay_us(uint32_t us)
{
	volatile uint32_t n = us * 4UL;
	while (n--);
}

static void delay_ms(uint32_t ms)
{
	uint32_t start = MCAL_TIM2_GetMs();
	while (!MCAL_TIM2_Elapsed(start, ms));
}

static void expander_write(uint8_t data)
{
	uint8_t b = data | LCD_BL;
	MCAL_I2C1_Write(LCD_I2C_ADDR, &b, 1);
}

// 'nibble' holds D7..D4 in the upper 4 bits, control bits in the lower bits
static void pulse_nibble(uint8_t nibble)
{
	expander_write(nibble | LCD_EN);
	delay_us(2);
	expander_write(nibble & ~LCD_EN);
	delay_us(50);
}

static void lcd_send(uint8_t value, uint8_t is_data)
{
	uint8_t rs = is_data ? LCD_RS : 0;
	pulse_nibble((value & 0xF0) | rs);            // high nibble first
	pulse_nibble(((value << 4) & 0xF0) | rs);     // then low nibble
}

static void lcd_cmd(uint8_t cmd)
{
	lcd_send(cmd, 0);
}

void LCD_Init(void)
{
	expander_write(0);
	delay_ms(50);

	// HD44780 4-bit initialization sequence
	pulse_nibble(0x30); delay_ms(5);
	pulse_nibble(0x30); delay_us(150);
	pulse_nibble(0x30); delay_us(150);
	pulse_nibble(0x20);                 // switch to 4-bit mode

	lcd_cmd(0x28);                      // 4-bit, 2 lines, 5x8 font
	lcd_cmd(0x08);                      // display off
	LCD_Clear();
	lcd_cmd(0x06);                      // entry mode: increment, no shift
	lcd_cmd(0x0C);                      // display on, cursor off
}

void LCD_Clear(void)
{
	lcd_cmd(0x01);
	delay_ms(3);                        // clear needs ~1.52 ms
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
	uint8_t addr = (row == 0) ? 0x00 : 0x40;
	lcd_cmd(0x80 | (addr + col));
}

void LCD_PrintString(const char *str)
{
	while (*str)
	{
		lcd_send((uint8_t)*str, 1);
		str++;
	}
}
