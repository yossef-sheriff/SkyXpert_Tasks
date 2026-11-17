/*
 * main.c
 *
 * LED toggle using an EXTI push-button interrupt (STM32F103C8 - Blue Pill)
 *
 * Wiring:
 *   LED    -> PC13 (on-board LED, ACTIVE LOW: pin = 0 -> LED ON)
 *   Button -> PA0 on one side, GND on the other side
 *             (internal pull-up is used, so the pin reads 1 idle, 0 pressed)
 */

#include "stm32f103x8.h"
#include "GPIO.h"
#include "EXTI.h"

#define LED_PORT		GPIO_C
#define LED_PIN			GPIO_PIN_13

#define BTN_PORT		GPIO_A
#define BTN_PIN			GPIO_PIN_0

// Set inside the ISR, consumed in the main loop
static volatile uint8_t button_event = 0;

static void delay_debounce(void)
{
	// Rough software delay (~20-30 ms at the default 8 MHz HSI clock)
	for (volatile uint32_t i = 0; i < 50000; i++);
}

// Called from EXTI0_IRQHandler -> keep it short
static void Button_Callback(void)
{
	button_event = 1;
}

static void LED_Init(void)
{
	GPIO_PinConfig_t cfg;

	RCC_GPIO_C_CLK_EN();

	cfg.GPIO_PinNumber   = LED_PIN;
	cfg.GPIO_MODE        = GPIO_MODE_OUTPUT_PP;
	cfg.GPIO_Output_Speed = GPIO_SPEED_2M;
	MCAL_GPIO_Init(LED_PORT, &cfg);

	// LED is active low -> write 0 to turn it ON (initial state = ON)
	MCAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
}

static void Button_Init(void)
{
	GPIO_PinConfig_t cfg;
	EXTI_PinConfig_t exti;

	RCC_GPIO_A_CLK_EN();

	// PA0 as input with pull-up
	cfg.GPIO_PinNumber = BTN_PIN;
	cfg.GPIO_MODE      = GPIO_MODE_INPUT_PU;
	MCAL_GPIO_Init(BTN_PORT, &cfg);

	// EXTI0 on PA0, falling edge (button press connects the pin to GND)
	exti.EXTI_Pin       = EXTI0PA0;
	exti.Trigger_case   = EXTI_TRIGGER_FALLING;
	exti.IRQ_EN         = EXTI_LINE_ENABLED;
	exti.P_IRQ_Callback = Button_Callback;
	MCAL_EXTI_Init(exti);
}

int main(void)
{
	LED_Init();
	Button_Init();

	while (1)
	{
		if (button_event)
		{
			delay_debounce();

			// Confirm the button is really still pressed (filters noise)
			if (MCAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_RESET)
			{
				MCAL_GPIO_TogglePin(LED_PORT, LED_PIN);

				// Wait for release so one press = one toggle
				while (MCAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_RESET);
				delay_debounce();
			}

			button_event = 0;
		}
	}
}
