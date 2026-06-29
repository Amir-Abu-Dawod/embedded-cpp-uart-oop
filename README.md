# Embedded C++ UART OOP Design

This project demonstrates an embedded-style UART communication design written in modern C++, focused on object-oriented design, hardware abstraction, buffering, and testability.

The project is inspired by a hardware-verified STM32 UART project implemented in C, where USART2 RX interrupts, a custom ring buffer, USB-UART communication, and PulseView logic analyzer decoding were used to validate real UART communication on STM32F407G-DISC1.

This version focuses on C++ software design before hardware integration. It replaces the real UART peripheral with a `MockUart` implementation, allowing the UART flow to be tested on a PC without STM32 hardware.

## What This Project Demonstrates

* Object-oriented C++ design for embedded-style communication software
* UART hardware abstraction using an `IUart` interface
* Mock-based testing using `MockUart`
* Fixed-size `RingBuffer` implementation without dynamic allocation
* RX/TX buffering using composition
* FIFO behavior, wraparound, overflow handling, and zero-capacity safety
* TX backpressure handling using `peek()` before `pop()` to avoid data loss
* Testable design that can later be adapted to STM32CubeIDE or real embedded hardware

## Architecture Flow

```text
Test / Mock PC input
   ↓
MockUart RX queue
   ↓
BufferedUart::pollRx()
   ↓
RingBuffer RX buffer
   ↓
BufferedUart::echoAvailable()
   ↓
RingBuffer TX buffer
   ↓
BufferedUart::flushTx()
   ↓
MockUart TX queue
   ↓
Test verifies echoed output
```

## Main Components

* `IUart` — abstract UART interface used to decouple application logic from hardware
* `MockUart` — PC-side simulated UART implementation for testing
* `RingBuffer` — fixed-size FIFO byte buffer with overflow protection
* `BufferedUart` — UART wrapper that adds RX/TX software buffering
* `tests/main.cpp` — test cases covering FIFO order, wraparound, bulk read/write, overflow, echo flow, and TX backpressure

## Build and Run

```bash
g++ -std=c++17 -Wall -Wextra -Werror -Iinclude src/RingBuffer.cpp src/MockUart.cpp src/BufferedUart.cpp tests/main.cpp -o oop_uart
./oop_uart
```

## Relation to Embedded Firmware Work

This project complements my STM32 bare-metal firmware portfolio by showing how embedded communication logic can be designed in a modular, testable C++ style before being connected to real hardware.
