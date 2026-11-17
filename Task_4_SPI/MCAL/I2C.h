/*
 * I2C.h
 *
 * I2C1 master, polling, standard mode 100 kHz.
 * PB6 = SCL, PB7 = SDA (open-drain alternate function).
 */
#ifndef MCAL_I2C_H_
#define MCAL_I2C_H_

#include "stm32f103x8.h"
#include "config.h"

#define I2C_OK		1
#define I2C_ERR		0

void    MCAL_I2C1_Init(void);
uint8_t MCAL_I2C1_Write(uint8_t addr7, const uint8_t *data, uint8_t len);

#endif /* MCAL_I2C_H_ */
