/*
 * UART.h
 *
 * USART1: PA9 = TX, PA10 = RX, 8N1, 9600 baud.
 * RX is interrupt driven and stored in a ring buffer.
 */
#ifndef MCAL_UART_H_
#define MCAL_UART_H_

#include "stm32f103x8.h"
#include "config.h"

// BRR = PCLK / baud (rounded). Oversampling x16 is already included in this formula.
#define USART_BRR_VALUE(pclk, baud)		(((pclk) + ((baud) / 2UL)) / (baud))

void    MCAL_UART1_Init(void);
void    MCAL_UART1_SendByte(uint8_t byte);
uint8_t MCAL_UART1_Available(void);
uint8_t MCAL_UART1_ReadByte(void);

#endif /* MCAL_UART_H_ */
