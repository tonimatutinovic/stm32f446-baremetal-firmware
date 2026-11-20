# STM32F446RE Bare-Metal Firmware

A hands-on embedded C project focused on register-level peripheral programming, interrupt-driven communication, and modular firmware development on the STM32F446RE microcontroller.

The goal is to build a reliable, well-documented firmware platform while developing a practical understanding of embedded C, hardware peripherals, interrupt handling, and low-level software architecture.

The project is developed incrementally, with each feature implemented and tested on physical hardware.

## Hardware and Development Environment

- **Development board:** STM32 NUCLEO-F446RE
- **Microcontroller:** STM32F446RE (ARM Cortex-M4)
- **IDE:** STM32CubeIDE
- **Programming and debugging:** On-board ST-LINK
- **Serial communication:** USART2 via ST-LINK Virtual COM Port
- **UART configuration:** 115200 baud, 8 data bits, no parity, 1 stop bit (8N1)

The firmware uses CMSIS device register definitions for direct peripheral access. STM32CubeIDE-generated initialization code remains in the project, while the custom peripheral drivers operate through hardware registers rather than HAL peripheral APIs.

## Implemented Features

### GPIO Driver

Custom register-level GPIO driver supporting:

- GPIO peripheral clock enable
- Output pin configuration
- Setting and resetting individual pins
- Pin toggling
- Setting and resetting multiple pins using bit masks
- Basic argument validation

The on-board LED (PA5) is controlled using this driver.

### USART2 Driver

Custom interrupt-driven USART2 driver supporting:

- Register-level USART2 configuration
- TX on PA2 and RX on PA3 using alternate function AF7
- 115200 baud communication
- Interrupt-driven reception using RXNE
- Interrupt-driven transmission using TXE
- Separate RX and TX ring buffers
- Non-blocking transmission API
- Software RX buffer overflow detection
- Hardware USART overrun error counting
- TX buffer overflow counting

**RX ring buffer:**
- Allocated size: 8 bytes
- Usable capacity: 7 bytes

**TX ring buffer:**
- Allocated size: 128 bytes
- Usable capacity: 127 bytes

Both buffers reserve one slot to distinguish between empty and full states.

The application enqueues outgoing data using `uart2_write()` or `uart2_write_string()`. The USART2 interrupt handler transfers queued bytes to the hardware transmit data register when it becomes available.

The TXE interrupt is disabled when the transmission queue is empty, preventing unnecessary repeated interrupts.

The non-blocking write API returns the number of bytes accepted into the software buffer, not the number physically transmitted.

### TIM2 Driver

Custom timer driver supporting:

- Register-level TIM2 configuration
- Periodic timer interrupts
- Interrupt-to-main-loop event signaling

The current demonstration generates a timer event every 500 ms, which is used to toggle the on-board LED.

### EXTI Driver

External interrupt handling for the on-board user button (PC13), including:

- External interrupt configuration
- Interrupt event signaling
- Software debounce using a 30 ms interval

Button events are processed in the main application loop rather than performing UART transmission directly inside the button interrupt handler.

### SysTick Timekeeping

Millisecond timekeeping used for:

- Button debounce timing
- Application-level elapsed-time measurements

## Software Architecture

The firmware separates peripheral configuration, interrupt handling, and application logic.

**Application layer (`main.c`)**
- Initializes the custom drivers
- Processes received UART data
- Handles timer and button events
- Enqueues outgoing UART messages

**Driver layer**
- `gpio_driver` — GPIO control
- `uart_driver` — USART2 communication and buffering
- `timer_driver` — TIM2 configuration and events
- `exti_driver` — External interrupt handling
- `systick_driver` — Millisecond timekeeping

**Interrupt handlers**
- Handle hardware events
- Update driver state and event flags
- Transfer UART data between hardware registers and software buffers

The main loop remains responsive without waiting for individual UART bytes to finish transmitting.

## Current Demonstration

The firmware currently performs the following operations:

1. Initializes GPIO, USART2, TIM2, and EXTI13.
2. Sends startup messages through USART2.
3. Echoes received UART characters.
4. Toggles the on-board LED every 500 ms.
5. Sends a message when the user button is pressed.

Example serial output:

```text
UART + TIM2 ready!
EXTI13 ready!
Button pressed!
```

Characters sent from a serial terminal are echoed back through the interrupt-driven UART driver.

## Build and Run

1. Clone the repository.
2. Open or import the project in STM32CubeIDE.
3. Connect the NUCLEO-F446RE using the on-board ST-LINK USB connection.
4. Build the project.
5. Flash and run the firmware.
6. Open the ST-LINK Virtual COM Port using a serial terminal configured for **115200 baud, 8N1**.
7. Send characters to test UART echo.
8. Press the user button to generate a UART message.
9. Observe the on-board LED toggling periodically.

## Testing Status

The following functionality has been verified on physical NUCLEO-F446RE hardware:

- Successful firmware build and flashing
- USART2 startup messages
- UART character echo
- Interrupt-driven UART transmission
- Button-triggered UART messages
- Periodic LED toggling using TIM2

Further testing is planned for:

- RX and TX buffer saturation
- Overflow counter behavior
- Hardware overrun conditions
- Sustained UART traffic
- Error handling and recovery
- Driver behavior under interrupt load

## Known Limitations

- The UART transmit API may accept fewer bytes than requested when the TX buffer is full.
- The current UART driver does not provide automatic retransmission of discarded bytes.
- The blocking `uart2_read_char()` API is not intended for concurrent use with interrupt-driven reception.
- Long critical sections in UART transmission may delay reception interrupt handling.
- Buffer saturation and sustained-traffic behavior require additional validation.

These limitations will be addressed or further documented as the project develops.

## Roadmap

- [x] Register-level GPIO driver
- [x] USART2 initialization and basic communication
- [x] Interrupt-driven UART reception
- [x] UART RX ring buffer
- [x] TIM2 periodic interrupts
- [x] EXTI13 button interrupt and debounce
- [x] SysTick-based timekeeping
- [x] Interrupt-driven, non-blocking UART transmission
- [x] UART TX ring buffer
- [ ] UART robustness and edge-case testing
- [ ] UART command-line interface
- [ ] SPI peripheral driver
- [ ] I2C peripheral driver
- [ ] DMA-based data transfers
- [ ] Improved error handling and fault diagnostics
- [ ] Additional testing and technical documentation

## Project Structure

- `Core/Inc/` — Application and driver header files
- `Core/Src/` — Application and driver implementations
- `Core/Startup/` — Microcontroller startup code
- `Drivers/` — STM32 HAL and CMSIS support files

## Development Approach

This project emphasizes understanding hardware behavior rather than relying exclusively on high-level peripheral libraries.

Each feature is developed through:

1. Studying the relevant peripheral registers and hardware behavior.
2. Implementing a custom driver in embedded C.
3. Testing functionality on physical hardware.
4. Reviewing edge cases and potential failure conditions.
5. Documenting implementation decisions and limitations.
6. Tracking changes using Git.

The long-term objective is a modular, reliable, and well-documented embedded firmware project that demonstrates practical low-level development skills.
