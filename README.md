# Motion copy Robot hand – Atmega88 + STM32

A motion copy robot hand using Atmega88 and STM32 microcontrollers, flex sensors and DC motors.
Firmware written in C, connected via Bluetooth between sensor and robot.

## 🔧 Features
- Robot hand movement controlled via DC motors
- Bluetooth connection with FB155BC
- Motion sensing via Flex sensors

## 🧩 Hardware Specs
- Atmega88 microcontroller
- 74125 tri-state buffer
- flex sensor
- STM32F103VBT6 microcontroller
- L298N Motor Driver
- ULN2803 voltage transfer buffer
- FB155BC Bluetooth module
- DC Geared motor
- Servo motor

## 📂 Repository Contents
- `/firmware`: Embedded C firmware source for STM32, Atmega88
- `/docs`: Schematic (PDF), part list, assembly & debug notes
- `/images`: Photos and waveforms

## 🛠 Build Instructions
1. Connect FB155BC via UART to STM32/Atmega88
2. Connect Flex sensors to ADC ports of Atmega88
3. Connect DC geared motors to STM32 I/O ports and Servo motors to PWM ports
4. Flash firmware using Keil Complier(STM32) and CodeVision(Atmega88)
5. Adjust PID constants in `main.c` for your build
6. Power the system and test motion copy

## 📸 Result
![Final Robot](images/final_product.jpg)

## 📝 License
This project is licensed under the MIT License.
