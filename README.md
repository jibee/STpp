STpp
====

This is a C++ framework for STM32F4 chips, based on ST-provided stm32f4xx.h

The frameworks provides a low-level abstraction of typical hardware components
such as DMA channels, Timers, GPIOs, Interrupts, ADCs, and Buses. This
low-level interface is located in the plat-inc and plat-stm folders in the
Platform namespace.

On top of this abstraction layer we also have a series of higher level drivers
which implement the protocol expected by the device. Examples are the HBridge
driver, i2c compass, lidar, ESP8266 wifi serial bridge...


Project Organisation
--------------------

The folders are (most of the time) organized as follow:

- plat-\* contain low level drivers. There is plat-unix which is are just stubs, and plat-stm with actual code.
plat-inc are the matching headers.
The philosphy behind them is to be a close match to actual STM32F4 registers.
It hides the access to RCC clocks though, and there are some helper functions.

- lib contain higher level drivers. They are for components external to the STM32F4 itself.

Current state
=============

Supported drivers:
- AX-12 (one half-duplex wire, three wires, En + Tx + Rx)
- Bluetooth
- Three wires Half H-Bridge (PWM + EnA + EnB)
- HD44780-like lcd
- WS82(1112b)
- Neato XV11 Lidar

Supported STM32F4 function:
- Adc (only one shot, no DMA)
- DMA
- EXTI
- GPIO
- NVIC
- SPI
- Timer
- Uart
- UsbSerial
- Watchdog
