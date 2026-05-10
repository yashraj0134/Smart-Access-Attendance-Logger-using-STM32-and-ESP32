# Smart Access & Attendance Logger using STM32 and ESP32

## Overview
An embedded smart access and attendance logging system developed using STM32 and ESP32 microcontrollers. The system uses RFID authentication for secure access control and real-time attendance tracking with cloud synchronization.

## Features
- RFID-based user authentication
- Real-time attendance logging
- STM32 and ESP32 dual-controller architecture
- UART communication between controllers
- RTC-based timestamp management
- LCD status display
- WiFi-based cloud synchronization
- Blynk dashboard integration
- Entry/Exit monitoring system

## Hardware Used
- STM32F446RET6
- ESP32 DevKit V1
- MFRC522 RFID Module
- RTC Module
- 16x2 LCD
- UART Communication Interface
- WiFi Connectivity

## Communication Protocols
- UART
- SPI
- I2C

## Software & Tools
- Embedded C
- STM32CubeIDE
- Arduino IDE
- Blynk IoT Platform

## System Workflow
1. RFID card is scanned
2. STM32 validates RFID UID
3. Attendance timestamp generated using RTC
4. Data sent to ESP32 through UART
5. ESP32 uploads attendance data to cloud dashboard
6. LCD displays access status

## Project Objectives
- Automate attendance management
- Improve access security
- Enable real-time remote monitoring
- Demonstrate embedded-to-IoT integration

## Future Improvements
- Fingerprint authentication
- Database integration
- MQTT-based cloud communication
- Face recognition support

## Author
Yashraj — Embedded Systems & Firmware Development Enthusiast
