/*
 * EXTI.h
 *
 *  Created on: Jul 31, 2026
 *      Author: Mohamed A. Ragab
 */

#ifndef EXTI_H_
#define EXTI_H_

#include "stm32f103x8.h"
#include "GPIO.h"

/*******************************************************************************
                        EXTI Line & IRQ Definitions
 *******************************************************************************/
// EXTI Line Numbers (Used for IMR, EMR, RTSR, FTSR, PR registers)
#define EXTI0                   0
#define EXTI1                   1
#define EXTI2                   2
#define EXTI3                   3
#define EXTI4                   4
#define EXTI5                   5
#define EXTI6                   6
#define EXTI7                   7
#define EXTI8                   8
#define EXTI9                   9
#define EXTI10                  10
#define EXTI11                  11
#define EXTI12                  12
#define EXTI13                  13
#define EXTI14                  14
#define EXTI15                  15
#define EXTI16                  16 // PVD
#define EXTI17                  17 // RTC Alarm
#define EXTI18                  18 // USB Wakeup
#define EXTI19                  19 // Reserved (Ethernet Wakeup)

// EXTI NVIC Request Position Macros
#define EXTI0_IRQ               6
#define EXTI1_IRQ               7
#define EXTI2_IRQ               8
#define EXTI3_IRQ               9
#define EXTI4_IRQ               10
#define EXTI5_IRQ               23
#define EXTI6_IRQ               23
#define EXTI7_IRQ               23
#define EXTI8_IRQ               23
#define EXTI9_IRQ               23
#define EXTI9_5_IRQ             23
#define EXTI10_IRQ              40
#define EXTI11_IRQ              40
#define EXTI12_IRQ              40
#define EXTI13_IRQ              40
#define EXTI14_IRQ              40
#define EXTI15_IRQ              40
#define EXTI15_10_IRQ           40

// Internal Peripheral EXTI NVIC Positions
#define EXTI16_IRQ              1  // PVD_IRQ
#define EXTI17_IRQ              41 // RTCAlarm_IRQ
#define EXTI18_IRQ              42 // USBWakeUp_IRQ
#define EXTI19_IRQ              62 // Reserved for ETH_WKUP on higher density lines

/*******************************************************************************
                        NVIC and EXTI Mapping
 *******************************************************************************/
/*
 * If a pending interrupt is enabled, the NVIC activates the interrupt based on its priority. If an
   interrupt is not enabled, asserting its interrupt signal changes the interrupt state to pending,
   but the NVIC never activates the interrupt, regardless of its priority
 */
typedef struct{
	volatile uint32_t NVIC_ISER0;
	volatile uint32_t NVIC_ISER1;
	volatile uint32_t NVIC_ISER2;
}Set_enable_t;

/*
 * The ICER0-ICER2 registers disable interrupts, and show which interrupts are enabled.
 * */
typedef struct{
	volatile uint32_t NVIC_ICER0;
	volatile uint32_t NVIC_ICER1;
	volatile uint32_t NVIC_ICER2;
}Clear_enable_t;

/*
 * The ISPR0-ISPR2 registers force interrupts into the pending state, and show which
	interrupts are pending.
 * */
typedef struct{
	volatile uint32_t NVIC_ISPR0;
	volatile uint32_t NVIC_ISPR1;
	volatile uint32_t NVIC_ISPR2;
}Set_pending_t;

/*
 * The ICPR0-ICPR2 registers remove the pending state from interrupts, and show which
   interrupts are pending.
 * */
typedef struct{
	volatile uint32_t NVIC_ICPR0;
	volatile uint32_t NVIC_ICPR1;
	volatile uint32_t NVIC_ICPR2;
}Clear_pending_t;

/*
 * The IABR0-IABR2 registers indicate which interrupts are active.
 * */
typedef struct{
	volatile uint32_t NVIC_IABR0;
	volatile uint32_t NVIC_IABR1;
	volatile uint32_t NVIC_IABR2;
}Active_bit_t;

/*
 * The IPR0-IPR16 registers provide a 4-bit priority field for each interrupt. These registers are
   byte-accessible. Each register holds four priority fields, that map to four elements in the
   CMSIS interrupt priority array IP[0] to IP[67].
   ==> Required privilege: Privileged
 */
typedef struct{
	volatile uint8_t NVIC_IPR[21];
}NVIC_IPR_t;

#define NVIC_SET_ENABLE 		((Set_enable_t*)   			(Cortex_M3_NVIC + 0x000))
#define NVIC_CLEAR_ENABLE 		((Clear_enable_t*)  		(Cortex_M3_NVIC + 0x080))
#define NVIC_SET_PENDING 		((Set_pending_t*)   		(Cortex_M3_NVIC + 0x100))
#define NVIC_CLEAR_PENDING	 	((Clear_pending_t*) 		(Cortex_M3_NVIC + 0x180))
#define NVIC_ACTIVE_BIT 		((Active_bit_t*)    		(Cortex_M3_NVIC + 0x200))
#define NVIC_IPR				((NVIC_IPR_t*)	   			(Cortex_M3_NVIC + 0x300))
#define NVIC_SW_TRIGGER_INT		((volatile uint32_t*)	    (Cortex_M3_NVIC + 0xE00))

/*******************************************************************************
                        GPIO and EXTI Mapping
 *******************************************************************************/
typedef enum{
	EXTI_PORTA = 0,
	EXTI_PORTB,
	EXTI_PORTC,
	EXTI_PORTD
}EXTI_Port_t;

typedef enum{
	EXTI_TRIGGER_RISING,
	EXTI_TRIGGER_FALLING,
	EXTI_TRIGGER_BOTH
}EXTI_Trigger_t;

// EXTI Line State
#define EXTI_LINE_DISABLE	0
#define EXTI_LINE_ENABLED	1

typedef struct{
    uint16_t EXTI_InputLineNumber;
    GPIO_TypeDef* GPIO_Port; // Changed from GPIO_t to GPIO_TypeDef
    uint16_t GPIO_Pin;
    uint8_t IVT_IRQ_Num;
}EXTI_GPIO_Mapping_t;

typedef struct{
	EXTI_GPIO_Mapping_t EXTI_Pin;
	uint8_t Trigger_case;
	uint8_t IRQ_EN; 				// Enable or Disable the EXTI IRQ
	void (*P_IRQ_Callback)(void);
}EXTI_PinConfig_t;

/****************************** @ref EXTI_define ******************************/
#define EXTI0PA0		   (EXTI_GPIO_Mapping_t){EXTI0, GPIO_A, GPIO_PIN_0, EXTI0_IRQ}
#define EXTI0PB0           (EXTI_GPIO_Mapping_t){EXTI0, GPIO_B, GPIO_PIN_0, EXTI0_IRQ}
#define EXTI0PC0           (EXTI_GPIO_Mapping_t){EXTI0, GPIO_C, GPIO_PIN_0, EXTI0_IRQ}
#define EXTI0PD0           (EXTI_GPIO_Mapping_t){EXTI0, GPIO_D, GPIO_PIN_0, EXTI0_IRQ}

#define EXTI1PA1           (EXTI_GPIO_Mapping_t){EXTI1, GPIO_A, GPIO_PIN_1, EXTI1_IRQ}
#define EXTI1PB1           (EXTI_GPIO_Mapping_t){EXTI1, GPIO_B, GPIO_PIN_1, EXTI1_IRQ}
#define EXTI1PC1           (EXTI_GPIO_Mapping_t){EXTI1, GPIO_C, GPIO_PIN_1, EXTI1_IRQ}
#define EXTI1PD1           (EXTI_GPIO_Mapping_t){EXTI1, GPIO_D, GPIO_PIN_1, EXTI1_IRQ}

#define EXTI2PA2           (EXTI_GPIO_Mapping_t){EXTI2, GPIO_A, GPIO_PIN_2, EXTI2_IRQ}
#define EXTI2PB2           (EXTI_GPIO_Mapping_t){EXTI2, GPIO_B, GPIO_PIN_2, EXTI2_IRQ}
#define EXTI2PC2           (EXTI_GPIO_Mapping_t){EXTI2, GPIO_C, GPIO_PIN_2, EXTI2_IRQ}
#define EXTI2PD2           (EXTI_GPIO_Mapping_t){EXTI2, GPIO_D, GPIO_PIN_2, EXTI2_IRQ}

#define EXTI3PA3           (EXTI_GPIO_Mapping_t){EXTI3, GPIO_A, GPIO_PIN_3, EXTI3_IRQ}
#define EXTI3PB3           (EXTI_GPIO_Mapping_t){EXTI3, GPIO_B, GPIO_PIN_3, EXTI3_IRQ}
#define EXTI3PC3           (EXTI_GPIO_Mapping_t){EXTI3, GPIO_C, GPIO_PIN_3, EXTI3_IRQ}
#define EXTI3PD3           (EXTI_GPIO_Mapping_t){EXTI3, GPIO_D, GPIO_PIN_3, EXTI3_IRQ}

#define EXTI4PA4           (EXTI_GPIO_Mapping_t){EXTI4, GPIO_A, GPIO_PIN_4, EXTI4_IRQ}
#define EXTI4PB4           (EXTI_GPIO_Mapping_t){EXTI4, GPIO_B, GPIO_PIN_4, EXTI4_IRQ}
#define EXTI4PC4           (EXTI_GPIO_Mapping_t){EXTI4, GPIO_C, GPIO_PIN_4, EXTI4_IRQ}
#define EXTI4PD4           (EXTI_GPIO_Mapping_t){EXTI4, GPIO_D, GPIO_PIN_4, EXTI4_IRQ}

#define EXTI5PA5           (EXTI_GPIO_Mapping_t){EXTI5, GPIO_A, GPIO_PIN_5, EXTI5_IRQ}
#define EXTI5PB5           (EXTI_GPIO_Mapping_t){EXTI5, GPIO_B, GPIO_PIN_5, EXTI5_IRQ}
#define EXTI5PC5           (EXTI_GPIO_Mapping_t){EXTI5, GPIO_C, GPIO_PIN_5, EXTI5_IRQ}
#define EXTI5PD5           (EXTI_GPIO_Mapping_t){EXTI5, GPIO_D, GPIO_PIN_5, EXTI5_IRQ}

#define EXTI6PA6           (EXTI_GPIO_Mapping_t){EXTI6, GPIO_A, GPIO_PIN_6, EXTI6_IRQ}
#define EXTI6PB6           (EXTI_GPIO_Mapping_t){EXTI6, GPIO_B, GPIO_PIN_6, EXTI6_IRQ}
#define EXTI6PC6           (EXTI_GPIO_Mapping_t){EXTI6, GPIO_C, GPIO_PIN_6, EXTI6_IRQ}
#define EXTI6PD6           (EXTI_GPIO_Mapping_t){EXTI6, GPIO_D, GPIO_PIN_6, EXTI6_IRQ}

#define EXTI7PA7           (EXTI_GPIO_Mapping_t){EXTI7, GPIO_A, GPIO_PIN_7, EXTI7_IRQ}
#define EXTI7PB7           (EXTI_GPIO_Mapping_t){EXTI7, GPIO_B, GPIO_PIN_7, EXTI7_IRQ}
#define EXTI7PC7           (EXTI_GPIO_Mapping_t){EXTI7, GPIO_C, GPIO_PIN_7, EXTI7_IRQ}
#define EXTI7PD7           (EXTI_GPIO_Mapping_t){EXTI7, GPIO_D, GPIO_PIN_7, EXTI7_IRQ}

#define EXTI8PA8           (EXTI_GPIO_Mapping_t){EXTI8, GPIO_A, GPIO_PIN_8, EXTI8_IRQ}
#define EXTI8PB8           (EXTI_GPIO_Mapping_t){EXTI8, GPIO_B, GPIO_PIN_8, EXTI8_IRQ}
#define EXTI8PC8           (EXTI_GPIO_Mapping_t){EXTI8, GPIO_C, GPIO_PIN_8, EXTI8_IRQ}
#define EXTI8PD8           (EXTI_GPIO_Mapping_t){EXTI8, GPIO_D, GPIO_PIN_8, EXTI8_IRQ}

#define EXTI9PA9           (EXTI_GPIO_Mapping_t){EXTI9, GPIO_A, GPIO_PIN_9, EXTI9_IRQ}
#define EXTI9PB9           (EXTI_GPIO_Mapping_t){EXTI9, GPIO_B, GPIO_PIN_9, EXTI9_IRQ}
#define EXTI9PC9           (EXTI_GPIO_Mapping_t){EXTI9, GPIO_C, GPIO_PIN_9, EXTI9_IRQ}
#define EXTI9PD9           (EXTI_GPIO_Mapping_t){EXTI9, GPIO_D, GPIO_PIN_9, EXTI9_IRQ}

#define EXTI10PA10         (EXTI_GPIO_Mapping_t){EXTI10, GPIO_A, GPIO_PIN_10, EXTI10_IRQ}
#define EXTI10PB10         (EXTI_GPIO_Mapping_t){EXTI10, GPIO_B, GPIO_PIN_10, EXTI10_IRQ}
#define EXTI10PC10         (EXTI_GPIO_Mapping_t){EXTI10, GPIO_C, GPIO_PIN_10, EXTI10_IRQ}
#define EXTI10PD10         (EXTI_GPIO_Mapping_t){EXTI10, GPIO_D, GPIO_PIN_10, EXTI10_IRQ}

#define EXTI11PA11         (EXTI_GPIO_Mapping_t){EXTI11, GPIO_A, GPIO_PIN_11, EXTI11_IRQ}
#define EXTI11PB11         (EXTI_GPIO_Mapping_t){EXTI11, GPIO_B, GPIO_PIN_11, EXTI11_IRQ}
#define EXTI11PC11         (EXTI_GPIO_Mapping_t){EXTI11, GPIO_C, GPIO_PIN_11, EXTI11_IRQ}
#define EXTI11PD11         (EXTI_GPIO_Mapping_t){EXTI11, GPIO_D, GPIO_PIN_11, EXTI11_IRQ}

#define EXTI12PA12         (EXTI_GPIO_Mapping_t){EXTI12, GPIO_A, GPIO_PIN_12, EXTI12_IRQ}
#define EXTI12PB12         (EXTI_GPIO_Mapping_t){EXTI12, GPIO_B, GPIO_PIN_12, EXTI12_IRQ}
#define EXTI12PC12         (EXTI_GPIO_Mapping_t){EXTI12, GPIO_C, GPIO_PIN_12, EXTI12_IRQ}
#define EXTI12PD12         (EXTI_GPIO_Mapping_t){EXTI12, GPIO_D, GPIO_PIN_12, EXTI12_IRQ}

#define EXTI13PA13         (EXTI_GPIO_Mapping_t){EXTI13, GPIO_A, GPIO_PIN_13, EXTI13_IRQ}
#define EXTI13PB13         (EXTI_GPIO_Mapping_t){EXTI13, GPIO_B, GPIO_PIN_13, EXTI13_IRQ}
#define EXTI13PC13         (EXTI_GPIO_Mapping_t){EXTI13, GPIO_C, GPIO_PIN_13, EXTI13_IRQ}
#define EXTI13PD13         (EXTI_GPIO_Mapping_t){EXTI13, GPIO_D, GPIO_PIN_13, EXTI13_IRQ}

#define EXTI14PA14         (EXTI_GPIO_Mapping_t){EXTI14, GPIO_A, GPIO_PIN_14, EXTI14_IRQ}
#define EXTI14PB14         (EXTI_GPIO_Mapping_t){EXTI14, GPIO_B, GPIO_PIN_14, EXTI14_IRQ}
#define EXTI14PC14         (EXTI_GPIO_Mapping_t){EXTI14, GPIO_C, GPIO_PIN_14, EXTI14_IRQ}
#define EXTI14PD14         (EXTI_GPIO_Mapping_t){EXTI14, GPIO_D, GPIO_PIN_14, EXTI14_IRQ}

#define EXTI15PA15         (EXTI_GPIO_Mapping_t){EXTI15, GPIO_A, GPIO_PIN_15, EXTI15_IRQ}
#define EXTI15PB15         (EXTI_GPIO_Mapping_t){EXTI15, GPIO_B, GPIO_PIN_15, EXTI15_IRQ}
#define EXTI15PC15         (EXTI_GPIO_Mapping_t){EXTI15, GPIO_C, GPIO_PIN_15, EXTI15_IRQ}
#define EXTI15PD15         (EXTI_GPIO_Mapping_t){EXTI15, GPIO_D, GPIO_PIN_15, EXTI15_IRQ}

/*******************************************************************************
                        	NVIC Functions
 *******************************************************************************/

/*******************************************************************************
                        	EXTI Functions
 *******************************************************************************/
void Update_EXTI(EXTI_PinConfig_t* EXTI_Config);
void MCAL_EXTI_Init(EXTI_PinConfig_t EXTI_Config);
void MCAL_EXTI_Deinit(void);
void MCAL_Update_Init(EXTI_PinConfig_t* EXTI_Config);

#endif /* EXTI_H_ */
