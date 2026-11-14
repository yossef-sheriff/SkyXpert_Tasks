
Task #2: Inter-MCU Communication via SPI Protocol

Objective:
Replicate the full functionality of Task #1 by replacing the UART communication link with SPI.

Hardware Configuration:
- *MCU_1:* Configured as *SPI Master* + 16x2 LCD.
- *MCU_2:* Configured as *SPI Slave* + push-button + LED.
- *Communication Link:* SPI Bus (MOSI, MISO, SCK, NSS/CS) + Common GND.

Functional Requirements:

1. MCU_2 - LED Control & Interrupt Handling:
 - Identical to Task #1. LED blinks by default, toggles to HIGH on external interrupt, returns to blinking on next interrupt.

2. Inter-MCU Communication (SPI):
 - Master (MCU_1): Shall initiate data polling/reading from the slave periodically (e.g., every 100ms) or via NSS interrupt.
 - Slave (MCU_2): Shall prepare the LED status flag in its SPI data register (SPDR) to be transmitted upon master request. Use SPI interrupt to ensure non-blocking communication.
 - Handle NSS as hardware/software slave select.

3. MCU_1 - Display Logic:
 - Identical to Task #1. Display `LED is Blinking`, `Interrupt happened`, and `LED is HIGH` based on data received via SPI.

Key Difference to Consider:
Unlike UART (asynchronous), SPI is synchronous and master-driven. The implementation must ensure MCU_2 (slave) does not lose status updates between master polling cycles.

Deliverables: 
Source code (HAL/LL), updated wiring diagram for SPI, and timing diagram.
