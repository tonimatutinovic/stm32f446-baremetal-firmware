# STM32F446RE Bare-Metal Firmware

A hands-on embedded C project focused on register-level peripheral programming on the STM32F446RE microcontroller.

The goal is to build a modular firmware platform while learning low-level hardware control, embedded software architecture, and debugging.

## Hardware

- Board: NUCLEO-F446RE
- Microcontroller: STM32F446RE
- Development environment: STM32CubeIDE
- Programming and debugging: On-board ST-LINK

## Implemented Features

### GPIO Driver

Custom register-level GPIO driver supporting:

- GPIO peripheral clock enable
- Output pin configuration
- Setting and resetting individual pins
- Pin toggling
- Setting and resetting multiple pins using bit masks
- Basic argument validation

The driver accesses STM32 peripheral registers directly using CMSIS device definitions.

## Current Demo

The on-board LED (PA5) is controlled using the custom GPIO driver.

## Planned Features

- USART2 communication
- Interrupt-driven UART reception
- UART ring buffer
- Hardware timers
- UART command-line interface
- Firmware testing and documentation

## Project Structure

- `Core/Inc/` — Application and driver headers
- `Core/Src/` — Application and driver implementations
- `Core/Startup/` — Microcontroller startup code
- `Drivers/` — STM32/CMSIS support files

## Development Approach

The project emphasizes understanding peripheral registers and hardware behavior rather than relying on high-level HAL functions for custom driver implementations.

Features are developed incrementally, tested on physical hardware, and tracked using Git.