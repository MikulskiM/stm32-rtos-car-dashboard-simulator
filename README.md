# STM32 Car Dashboard Simulator

An embedded car dashboard simulator built on the STM32F411E-DISCO development board using STM32Cube HAL, C/C++, and PlatformIO. The goal of the project is to simulate features of a modern in-vehicle computer, such as a system menu UI, sensor data display, event logging (black box), lighting control, and more.

## 🚗 Features (Planned / In Progress)

- UART debug output and error logging
- Modular project structure in C++
- FreeRTOS-based task structure
- Menu UI controlled via rotary encoder with button
- OLED display output connected via I2C
- "Car Status" screen: voltage, tilt (IMU), temperature
- "Radio" screen: Simulated radio or music mode
- "Settings" screen (brightness, logging rate, etc.)
- LED lighting control menu (RGB strips or simulation)
- Black box style data logger (RAM or SD card)

## 🧠 Technologies Used

- STM32F411E-DISCO (STM32F411VET6 MCU)
- STM32Cube HAL
- PlatformIO
- C / C++
- UART debugging
- FreeRTOS
- I2C OLED
- rotary encoder input

## 🔧 How to Build

1. Clone this repo
2. Open in [VS Code + PlatformIO](https://platformio.org/)
3. Connect your STM32F411E-DISCO via USB (ST-LINK)
4. Click "Upload" or run:

```bash
pio run --target upload
```

## 📸 Preview

// TODO:

------------------------------
This project is for educational and prototyping purposes only and is not intended for direct use in automotive systems.
