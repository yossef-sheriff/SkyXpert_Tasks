/*
 * main_slave.c
 *
 * MCU_2 (Slave / Control unit)
 *   LED    : PA5 (external, active high)
 *   Button : PA0 to GND (internal pull-up, EXTI0 falling edge)
 *   UART   : PA9 = TX -> master RX
 */
#include "config.h"

#if defined(MCU_ROLE_SLAVE)

#include "stm32f103x8.h"
#include "GPIO.h"
#include "EXTI.h"
#include "TIMER.h"
#include "UART.h"

#define LED_PORT			GPIO_A
#define LED_PIN				GPIO_PIN_5

#define BTN_PORT			GPIO_A
#define BTN_PIN				GPIO_PIN_0

#define BLINK_HALF_PERIOD_MS	500UL
#define DEBOUNCE_MS				20UL
#define BTN_LOCKOUT_MS			200UL     // ignore any bounce burst after an accepted press

typedef enum
{
	MODE_BLINKING = 0,
	MODE_HIGH
} led_mode_t;

static led_mode_t led_mode = MODE_BLINKING;
static uint32_t blink_last = 0;

// Set by ISR, consumed by main loop
static volatile uint8_t  btn_pending = 0;
static volatile uint32_t btn_time = 0;
static volatile uint32_t last_accept_ms = 0;

static void LED_Set(uint8_t on)
{
	MCAL_GPIO_WritePin(LED_PORT, LED_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void LED_Init(void)
{
	GPIO_PinConfig_t cfg;

	RCC_GPIO_A_CLK_EN();

	cfg.GPIO_PinNumber    = LED_PIN;
	cfg.GPIO_MODE         = GPIO_MODE_OUTPUT_PP;
	cfg.GPIO_Output_Speed = GPIO_SPEED_2M;
	MCAL_GPIO_Init(LED_PORT, &cfg);

	LED_Set(0);
}

// EXTI0 callback: keep it short, only timestamp and flag
static void BTN_ISR(void)
{
	uint32_t now = MCAL_TIM2_GetMs();

	if ((now - last_accept_ms) < BTN_LOCKOUT_MS)
	{
		return;
	}

	btn_time = now;
	btn_pending = 1;
}

static void Button_Init(void)
{
	GPIO_PinConfig_t cfg;
	EXTI_PinConfig_t exti;

	RCC_GPIO_A_CLK_EN();

	cfg.GPIO_PinNumber    = BTN_PIN;
	cfg.GPIO_MODE         = GPIO_MODE_INPUT_PU;
	cfg.GPIO_Output_Speed = GPIO_SPEED_2M;
	MCAL_GPIO_Init(BTN_PORT, &cfg);

	exti.EXTI_Pin       = EXTI0PA0;
	exti.Trigger_case   = EXTI_TRIGGER_FALLING;
	exti.IRQ_EN         = EXTI_LINE_ENABLED;
	exti.P_IRQ_Callback = BTN_ISR;
	MCAL_EXTI_Init(exti);
}

static void Send_State(uint8_t state)
{
	MCAL_UART1_SendByte(PROTO_EVENT_INTERRUPT);
	MCAL_UART1_SendByte(state);
}

// Software debounce: confirm the press only if the line is still low
// DEBOUNCE_MS after the last falling edge.
static void Button_Task(void)
{
	if (!btn_pending || !MCAL_TIM2_Elapsed(btn_time, DEBOUNCE_MS))
	{
		return;
	}

	btn_pending = 0;

	if (MCAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) != GPIO_PIN_RESET)
	{
		return;                                   // noise / released too early
	}

	last_accept_ms = MCAL_TIM2_GetMs();

	if (led_mode == MODE_BLINKING)
	{
		led_mode = MODE_HIGH;
		LED_Set(1);
		Send_State(PROTO_STATE_HIGH);
	}
	else
	{
		led_mode = MODE_BLINKING;
		blink_last = MCAL_TIM2_GetMs();
		LED_Set(1);
		Send_State(PROTO_STATE_BLINKING);
	}
}

static void Blink_Task(void)
{
	if (led_mode != MODE_BLINKING)
	{
		return;
	}

	if (MCAL_TIM2_Elapsed(blink_last, BLINK_HALF_PERIOD_MS))
	{
		blink_last = MCAL_TIM2_GetMs();
		MCAL_GPIO_TogglePin(LED_PORT, LED_PIN);
	}
}

int main(void)
{
	MCAL_TIM2_Init();
	MCAL_UART1_Init();
	LED_Init();
	Button_Init();

	blink_last = MCAL_TIM2_GetMs();

	while (1)
	{
		Button_Task();
		Blink_Task();
	}
}

#endif /* MCU_ROLE_SLAVE */
