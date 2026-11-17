Task #1: Inter-MCU Communication via UART with Interrupt-Driven LED Control and LCD Status Display

Objective:
Establish UART communication between two STM32F103 microcontrollers to control and monitor LED states.

Hardware Configuration:
- *MCU_1 (Master/Display Unit):* STM32F103 interfaced with a 16x2 character LCD.
- *MCU_2 (Slave/Control Unit):* STM32F103 interfaced with a push-button and an LED.
- *Communication Link:* UART (e.g., USART1) between MCU_1 and MCU_2 (TX-RX, RX-TX, Common GND).

Functional Requirements:

MCU_2 - LED Control & Interrupt Handling:
*Default State:* The LED shall operate in blinking mode (e.g., 500ms ON / 500ms OFF).
*Interrupt Configuration:* Configure the push-button pin as an external interrupt (EXTI) triggered on the falling edge (with internal pull-up).
ISR Logic:
On 1st button press (interrupt), set a status flag LED_STATE = HIGH. Exit blinking mode and set LED to continuous HIGH. Enter ISR to execute the state change.
On 2nd button press (interrupt), clear the flag LED_STATE = BLINKING and return the LED to blinking mode.
Debouncing should be handled in software.

Inter-MCU Communication:
MCU_2 shall transmit the current LED status flag to MCU_1 via UART upon every state change.
Communication protocol: 8-bit data, 9600/115200 baud rate, No parity.

MCU_1 - Display Logic:
MCU_1 shall continuously receive status data from MCU_2 via UART interrupt (RXNE).
Based on the received flag, the LCD shall display the corresponding event:
LED is Blinking
When in default blinking mode.
Interrupt happened
Transient message for 2 seconds upon receiving an interrupt event.
LED is HIGH
When LED is set to continuous ON state.
