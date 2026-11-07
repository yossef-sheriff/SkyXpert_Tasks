/*
 * EXTI.c
 *
 *  Created on: Aug 23, 2026
 *      Author: Mohamed A. Ragab
 */

#include "EXTI.h"
#include <stddef.h>

// Initialize callback array to NULL to ensure safe execution checks
void (*GP_IRQCallBack[16])(void) = {NULL};

void EXTI0_IRQHandler(void){
	// Clear by writing 1 directly to avoid read-modify-write clearing of other pending bits
	EXTI->EXTI_PR = (1<<EXTI0);
	if(GP_IRQCallBack[EXTI0] != NULL) {
		GP_IRQCallBack[EXTI0]();
	}
}

void EXTI1_IRQHandler(void){
	EXTI->EXTI_PR = (1<<EXTI1);
	if(GP_IRQCallBack[EXTI1] != NULL) {
		GP_IRQCallBack[EXTI1]();
	}
}

void EXTI2_IRQHandler(void){
	EXTI->EXTI_PR = (1<<EXTI2);
	if(GP_IRQCallBack[EXTI2] != NULL) {
		GP_IRQCallBack[EXTI2]();
	}
}

void EXTI3_IRQHandler(void){
	EXTI->EXTI_PR = (1<<EXTI3);
	if(GP_IRQCallBack[EXTI3] != NULL) {
		GP_IRQCallBack[EXTI3]();
	}
}

void EXTI4_IRQHandler(void){
	EXTI->EXTI_PR = (1<<EXTI4);
	if(GP_IRQCallBack[EXTI4] != NULL) {
		GP_IRQCallBack[EXTI4]();
	}
}

void EXTI9_5_IRQHandler(void){
	// Decoupled into independent 'if' statements to handle simultaneous multiplexed interrupts
	if(EXTI->EXTI_PR & (1<<EXTI5)){
		EXTI->EXTI_PR = (1<<EXTI5);
		if(GP_IRQCallBack[EXTI5] != NULL) GP_IRQCallBack[EXTI5]();
	}
	if(EXTI->EXTI_PR & (1<<EXTI6)){
		EXTI->EXTI_PR = (1<<EXTI6);
		if(GP_IRQCallBack[EXTI6] != NULL) GP_IRQCallBack[EXTI6]();
	}
	if(EXTI->EXTI_PR & (1<<EXTI7)){
		EXTI->EXTI_PR = (1<<EXTI7);
		if(GP_IRQCallBack[EXTI7] != NULL) GP_IRQCallBack[EXTI7]();
	}
	if(EXTI->EXTI_PR & (1<<EXTI8)){
		EXTI->EXTI_PR = (1<<EXTI8);
		if(GP_IRQCallBack[EXTI8] != NULL) GP_IRQCallBack[EXTI8]();
	}
	if(EXTI->EXTI_PR & (1<<EXTI9)){
		EXTI->EXTI_PR = (1<<EXTI9);
		if(GP_IRQCallBack[EXTI9] != NULL) GP_IRQCallBack[EXTI9]();
	}
}

void EXTI15_10_IRQHandler(void){
	// Decoupled into independent 'if' statements to handle simultaneous multiplexed interrupts
	if(EXTI->EXTI_PR & (1<<EXTI10)){
		EXTI->EXTI_PR = (1<<EXTI10);
		if(GP_IRQCallBack[EXTI10] != NULL) GP_IRQCallBack[EXTI10]();
	}
	if(EXTI->EXTI_PR & (1<<EXTI11)){
		EXTI->EXTI_PR = (1<<EXTI11);
		if(GP_IRQCallBack[EXTI11] != NULL) GP_IRQCallBack[EXTI11]();
	}
	if(EXTI->EXTI_PR & (1<<EXTI12)){
		EXTI->EXTI_PR = (1<<EXTI12);
		if(GP_IRQCallBack[EXTI12] != NULL) GP_IRQCallBack[EXTI12]();
	}
	if(EXTI->EXTI_PR & (1<<EXTI13)){
		EXTI->EXTI_PR = (1<<EXTI13);
		if(GP_IRQCallBack[EXTI13] != NULL) GP_IRQCallBack[EXTI13]();
	}
	if(EXTI->EXTI_PR & (1<<EXTI14)){
		EXTI->EXTI_PR = (1<<EXTI14);
		if(GP_IRQCallBack[EXTI14] != NULL) GP_IRQCallBack[EXTI14]();
	}
	if(EXTI->EXTI_PR & (1<<EXTI15)){
		EXTI->EXTI_PR = (1<<EXTI15);
		if(GP_IRQCallBack[EXTI15] != NULL) GP_IRQCallBack[EXTI15]();
	}
}


/* Private NVIC Helper Functions */
static void NVIC_Enable(uint8_t IRQ_Number) {
    if (IRQ_Number < 32) {
        NVIC_SET_ENABLE->NVIC_ISER0 = (1 << IRQ_Number);
    } else if (IRQ_Number < 64) {
        NVIC_SET_ENABLE->NVIC_ISER1 = (1 << (IRQ_Number - 32));
    } else if (IRQ_Number < 81) {
        NVIC_SET_ENABLE->NVIC_ISER2 = (1 << (IRQ_Number - 64));
    }
}

static void NVIC_Disable(uint8_t IRQ_Number) {
    if (IRQ_Number < 32) {
        NVIC_CLEAR_ENABLE->NVIC_ICER0 = (1 << IRQ_Number);
    } else if (IRQ_Number < 64) {
        NVIC_CLEAR_ENABLE->NVIC_ICER1 = (1 << (IRQ_Number - 32));
    } else if (IRQ_Number < 81) {
        NVIC_CLEAR_ENABLE->NVIC_ICER2 = (1 << (IRQ_Number - 64));
    }
}


void MCAL_EXTI_Deinit(){
	// Reset all EXTI core registers to their default state
	EXTI->EXTI_IMR = 0x00000000;
	EXTI->EXTI_EMR = 0x00000000;
	EXTI->EXTI_RTSR = 0x00000000;
	EXTI->EXTI_FTSR = 0x00000000;
	EXTI->EXTI_SWIER = 0x00000000;

	// Clear all pending flags by writing 1 to all bit positions (rc_w1 mechanism)
	EXTI->EXTI_PR = 0xFFFFFFFF;

	// Disable all related EXTI interrupt in the NVIC
	NVIC_Disable(EXTI0_IRQ);
	NVIC_Disable(EXTI1_IRQ);
	NVIC_Disable(EXTI2_IRQ);
	NVIC_Disable(EXTI3_IRQ);
	NVIC_Disable(EXTI4_IRQ);
	NVIC_Disable(EXTI9_5_IRQ);
	NVIC_Disable(EXTI15_10_IRQ);
}

void Update_EXTI(EXTI_PinConfig_t* EXTI_Config){
	// Resolve GPIO Port to AFIO Port Mapping Value
	// 1. Enable AFIO Clock (CRITICAL STEP)
	RCC_AFIO_CLK_EN();
	// Resolve GPIO Port to AFIO Port Mapping Value
	uint8_t afio_port_value = 0;

	if(EXTI_Config->EXTI_Pin.GPIO_Port == GPIO_A){
		afio_port_value = EXTI_PORTA;
	}
	else if(EXTI_Config->EXTI_Pin.GPIO_Port == GPIO_B){
		afio_port_value = EXTI_PORTB;
	}
	else if(EXTI_Config->EXTI_Pin.GPIO_Port == GPIO_C){
		afio_port_value = EXTI_PORTC;
	}
	else if(EXTI_Config->EXTI_Pin.GPIO_Port == GPIO_D){
		afio_port_value = EXTI_PORTD;
	}

	uint8_t line = EXTI_Config->EXTI_Pin.EXTI_InputLineNumber;

	// AFIO Routing Configuration
	// Since EXTICR1 through EXTICR4 are contiguous in memory, cast the struct address
	// to a uint32_t array for algorithmic index access
	volatile uint32_t* AFIO_EXTICR_Array = (volatile uint32_t*)&(AFIO->AFIO_EXTICR1);
	uint8_t register_idx = line / 4;
	uint8_t bit_shift = (line % 4) * 4;

	// Clear the 4 bits corresponding to this EXTI line, then set the new routing value
	AFIO_EXTICR_Array[register_idx] &= ~(0xF << bit_shift);
	AFIO_EXTICR_Array[register_idx] |= (afio_port_value << bit_shift);

	// Trigger Edge Configuration
	// Ensure a clean state by cleaning both edge registers for this specific line
	EXTI->EXTI_RTSR &= ~(1 << line);
	EXTI->EXTI_FTSR &= ~(1 << line);

	if(EXTI_Config->Trigger_case == EXTI_TRIGGER_RISING){
		EXTI->EXTI_RTSR |= (1 << line);
	}
	else if(EXTI_Config->Trigger_case == EXTI_TRIGGER_FALLING){
		EXTI->EXTI_FTSR |= (1 << line);
	}
	else if(EXTI_Config->Trigger_case == EXTI_TRIGGER_BOTH){
		EXTI->EXTI_RTSR |= (1 << line);
		EXTI->EXTI_FTSR |= (1 << line);
	}

	// Register the Application-Layer Callback
	GP_IRQCallBack[line] = EXTI_Config->P_IRQ_Callback;

	// Interrupt Enable / Disable Routing
	if(EXTI_Config->IRQ_EN){
		EXTI->EXTI_IMR |= (1<<line);
		NVIC_Enable(EXTI_Config->EXTI_Pin.IVT_IRQ_Num);
	}
	else{
		EXTI->EXTI_IMR &= ~(1<<line);
		NVIC_Disable(EXTI_Config->EXTI_Pin.IVT_IRQ_Num);
	}
}

void MCAL_EXTI_Init(EXTI_PinConfig_t EXTI_Config){
	// Pass the struct address to the unified update routine
	Update_EXTI(&EXTI_Config);
}

void MCAL_Update_Init(EXTI_PinConfig_t* EXTI_Config){
	// Pass the struct pointer directly to the unified update routine
	Update_EXTI(EXTI_Config);
}
