/*
 * config.h
 */
#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

// ===== Select the role of this MCU (define ONE only) =====
#define MCU_ROLE_MASTER
#define COMM_MODE_SPI

#if defined(MCU_ROLE_MASTER) && defined(MCU_ROLE_SLAVE)
#error "Define only one role: MCU_ROLE_MASTER or MCU_ROLE_SLAVE"
#endif

#if !defined(MCU_ROLE_MASTER) && !defined(MCU_ROLE_SLAVE)
#error "No MCU role defined"
#endif

// ===== Clock (HSI 8 MHz, reset default, no PLL) =====
#define SYSCLK_HZ           8000000UL
#define PCLK1_HZ            8000000UL   // APB1 prescaler = 1
#define PCLK2_HZ            8000000UL   // APB2 prescaler = 1

// ===== UART =====
#define UART_BAUD           9600UL

// ===== LCD (PCF8574 backpack, 7-bit address) =====
#define LCD_I2C_ADDR        0x27        // try 0x3F if LCD shows nothing

#define PROTO_EVENT_INTERRUPT       0x03    // a button press happened
#define PROTO_STATE_BLINKING        0x01    // LED is blinking
#define PROTO_STATE_HIGH            0x02    // LED is continuously ON
// ===== SPI frame =====
// Master sends:  [PROTO_SPI_CMD_POLL, 0x00, 0x00]
// Slave replies: [PROTO_SPI_HEADER, event, state]
//   event: PROTO_EVENT_INTERRUPT or 0x00
//   state: PROTO_STATE_BLINKING or PROTO_STATE_HIGH
#define PROTO_SPI_CMD_POLL			0xA5
#define PROTO_SPI_HEADER			0xB0

#endif /* INC_CONFIG_H_ */
