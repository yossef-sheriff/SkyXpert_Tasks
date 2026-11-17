/*
 * SPI.h
 *
 * SPI1 driver (register-level), Master or Slave, 8-bit, MSB first.
 *
 * Pins (remapped default):
 *   PA4 = NSS   PA5 = SCK   PA6 = MISO   PA7 = MOSI
 *
 * The driver does NOT control the chip-select in software-NSS master mode.
 * The application drives PA4 (or any GPIO) low/high around each transfer.
 */
#ifndef MCAL_SPI_H_
#define MCAL_SPI_H_

#include "stm32f103x8.h"

/*******************************************************************************
                        @ref SPI_Config_define
 *******************************************************************************/
#define SPI_MODE_SLAVE				0
#define SPI_MODE_MASTER				1

#define SPI_CPOL_LOW				0
#define SPI_CPOL_HIGH				1

#define SPI_CPHA_FIRST				0		// data sampled on first clock edge
#define SPI_CPHA_SECOND				1		// data sampled on second clock edge

// Baud rate = fPCLK2 / prescaler (fPCLK2 = 8 MHz with HSI and no APB prescaler)
#define SPI_BAUD_DIV2				0
#define SPI_BAUD_DIV4				1
#define SPI_BAUD_DIV8				2
#define SPI_BAUD_DIV16				3
#define SPI_BAUD_DIV32				4
#define SPI_BAUD_DIV64				5
#define SPI_BAUD_DIV128				6
#define SPI_BAUD_DIV256				7

#define SPI_NSS_SOFT				0		// SSM=1: CS handled by application
#define SPI_NSS_HW_INPUT			1		// SSM=0: slave, NSS pin is input
#define SPI_NSS_HW_OUTPUT			2		// SSM=0, SSOE=1: master drives NSS

#define SPI_IRQ_DISABLE				0
#define SPI_IRQ_ENABLE				1

/*******************************************************************************
                        Configuration Structure
 *******************************************************************************/
typedef struct
{
	uint8_t Mode;						// SPI_MODE_MASTER or SPI_MODE_SLAVE
	uint8_t Baud;						// SPI_BAUD_DIVx (master only)
	uint8_t CPOL;						// SPI_CPOL_LOW or SPI_CPOL_HIGH
	uint8_t CPHA;						// SPI_CPHA_FIRST or SPI_CPHA_SECOND
	uint8_t NSS;						// SPI_NSS_SOFT / SPI_NSS_HW_INPUT / SPI_NSS_HW_OUTPUT
	uint8_t IRQ_EN;						// SPI_IRQ_ENABLE: RXNE interrupt on
	void (*P_IRQ_Callback)(uint8_t rx);	// called from ISR with the received byte
}SPI_Config_t;

/*******************************************************************************
                        SPI Functions
 *******************************************************************************/
void    MCAL_SPI1_Init(SPI_Config_t* SPI_Config);
void    MCAL_SPI1_DeInit(void);

// Master only: full-duplex transfer of one byte. Blocking.
uint8_t MCAL_SPI1_TransferByte(uint8_t data);

// Loads the next byte into the TX buffer (DR). Safe to call from an ISR.
// Slave: call this to prepare the response for the next byte the master clocks.
void    MCAL_SPI1_SetTxData(uint8_t data);

uint8_t MCAL_SPI1_IsBusy(void);

#endif /* MCAL_SPI_H_ */
