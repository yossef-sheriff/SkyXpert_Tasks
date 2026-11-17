/*
 * TIMER.h
 *
 * TIM2 is used as a 1 ms system tick (non-blocking time base).
 */
#ifndef MCAL_TIMER_H_
#define MCAL_TIMER_H_

#include "stm32f103x8.h"
#include "config.h"

void     MCAL_TIM2_Init(void);
uint32_t MCAL_TIM2_GetMs(void);
uint8_t  MCAL_TIM2_Elapsed(uint32_t since, uint32_t duration);

#endif /* MCAL_TIMER_H_ */
