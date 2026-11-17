/*
 * LCD.h
 *
 * 16x2 HD44780 LCD in 4-bit mode through a PCF8574 I2C backpack.
 * Backpack mapping: P0=RS, P1=RW, P2=EN, P3=Backlight, P4..P7=D4..D7
 */
#ifndef HAL_LCD_H_
#define HAL_LCD_H_

#include "stm32f103x8.h"

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_PrintString(const char *str);

#endif /* HAL_LCD_H_ */
