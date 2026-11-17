/*
 * TIMER.c
 */
#include "TIMER.h"
#include "EXTI.h"          // NVIC_SET_ENABLE

static volatile uint32_t ms_ticks = 0;

void MCAL_TIM2_Init(void)
{
	RCC_TIM2_CLK_EN();

	// TIM2 clock = 8 MHz
	// PSC = 7999  -> counter clock = 1 kHz
	// ARR = 0     -> update event every 1 counter tick = 1 ms
	TIM2->PSC = (PCLK1_HZ / 1000UL) - 1UL;
	TIM2->ARR = 0;

	TIM2->EGR  |= (1<<0);      // UG: load PSC/ARR now
	TIM2->SR   &= ~(1<<0);     // clear UIF
	TIM2->DIER |= (1<<0);      // UIE: update interrupt enable
	TIM2->CR1  |= (1<<0);      // CEN: start counter

	NVIC_SET_ENABLE->NVIC_ISER0 = (1<<28);   // TIM2 IRQ = 28
}

uint32_t MCAL_TIM2_GetMs(void)
{
	return ms_ticks;
}

// Returns 1 when 'duration' ms have passed since 'since'
uint8_t MCAL_TIM2_Elapsed(uint32_t since, uint32_t duration)
{
	return ((ms_ticks - since) >= duration) ? 1 : 0;   // wrap-safe
}

void TIM2_IRQHandler(void)
{
	if (TIM2->SR & (1<<0))
	{
		TIM2->SR &= ~(1<<0);
		ms_ticks++;
	}
}
