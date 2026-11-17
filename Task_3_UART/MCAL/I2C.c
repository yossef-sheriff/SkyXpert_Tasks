/*
 * I2C.c
 */
#include "I2C.h"
#include "GPIO.h"

#define I2C_TIMEOUT_LOOPS	20000UL

static void i2c1_stop_on_error(void)
{
	I2C1->CR1 |= (1<<9);      // STOP
}

// Waits for 'cond' with a timeout. On timeout: send STOP and return I2C_ERR.
#define I2C_WAIT_UNTIL(cond)                          \
	do {                                              \
		uint32_t t_ = I2C_TIMEOUT_LOOPS;              \
		while (!(cond)) {                             \
			if (--t_ == 0) {                          \
				i2c1_stop_on_error();                 \
				return I2C_ERR;                       \
			}                                         \
		}                                             \
	} while (0)

void MCAL_I2C1_Init(void)
{
	GPIO_PinConfig_t cfg;

	RCC_GPIO_B_CLK_EN();
	RCC_I2C1_CLK_EN();

	// PB6 = SCL, PB7 = SDA: alternate function open-drain, 50 MHz
	cfg.GPIO_MODE         = GPIO_MODE_OUTPUT_AF_OD;
	cfg.GPIO_Output_Speed = GPIO_SPEED_50M;

	cfg.GPIO_PinNumber = GPIO_PIN_6;
	MCAL_GPIO_Init(GPIO_B, &cfg);

	cfg.GPIO_PinNumber = GPIO_PIN_7;
	MCAL_GPIO_Init(GPIO_B, &cfg);

	I2C1->CR1 = 0;                                       // disabled while configuring
	I2C1->CR2 = PCLK1_HZ / 1000000UL;                    // peripheral clock in MHz (8)
	I2C1->CCR = PCLK1_HZ / (2UL * 100000UL);             // standard mode 100 kHz -> 40
	I2C1->TRISE = (PCLK1_HZ / 1000000UL) + 1UL;          // 1000 ns / 125 ns + 1 = 9
	I2C1->CR1 |= (1<<0);                                 // PE: enable peripheral
}

uint8_t MCAL_I2C1_Write(uint8_t addr7, const uint8_t *data, uint8_t len)
{
	// START
	I2C1->CR1 |= (1<<8);
	I2C_WAIT_UNTIL(I2C1->SR1 & (1<<0));                  // SB

	// Address + write bit
	I2C1->DR = (uint8_t)(addr7 << 1);
	I2C_WAIT_UNTIL(I2C1->SR1 & ((1<<1) | (1<<10)));      // ADDR or AF

	if (I2C1->SR1 & (1<<10))                             // NACK: no device at this address
	{
		I2C1->SR1 &= ~(1<<10);                           // clear AF
		I2C1->CR1 |= (1<<9);                             // STOP
		return I2C_ERR;
	}

	// Clear ADDR: read SR1 then SR2
	{
		volatile uint32_t tmp = I2C1->SR1;
		tmp = I2C1->SR2;
		(void)tmp;
	}

	for (uint8_t i = 0; i < len; i++)
	{
		I2C_WAIT_UNTIL(I2C1->SR1 & (1<<7));              // TXE
		I2C1->DR = data[i];
	}

	I2C_WAIT_UNTIL(I2C1->SR1 & (1<<2));                  // BTF: last byte sent
	I2C1->CR1 |= (1<<9);                                 // STOP

	return I2C_OK;
}
