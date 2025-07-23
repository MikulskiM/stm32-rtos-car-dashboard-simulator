# STM32 Car Dashboard Simulator

An embedded car dashboard simulator built on the STM32F411E-DISCO development board using STM32Cube HAL, C/C++, and STM32CubeIDE. The goal of the project is to simulate features of a modern in-vehicle computer, such as a system menu UI, sensor data display, event logging (black box), lighting control, and more.

## 🚗 Features (Planned / In Progress)

- SWV debug output and error logging
- Modular project structure in C++
- FreeRTOS-based task structure
- Menu UI controlled via rotary encoder with button
- OLED display output connected via SPI
- "ACCEL" screen: Display accelerator data and detect free-falling
- "COMPASS" screen: Display the direction in which the device is heading
- "RADIO" screen: Simulated radio
- "SETTINGS" screen: Change display options
- "LED" screen: LED control menu (RGB strips and on-board LED)
- Black box style data logger (SD card)

## 🧠 Technologies Used

- STM32F411E-DISCO (STM32F411VET6 MCU)
- STM32CubeIDE + STM32CubeMX
- STM32Cube HAL
- C / C++
- SWV debugging
- FreeRTOS
- LSM303DLHC sensor
- ST7735 SPI OLED (0.96' 80x160)
- Rotary encoder with button

## 🔧 How to Build

1. Clone this repo
2. Open the project in **STM32CubeIDE**
3. Connect your STM32F411E-DISCO via USB (ST-LINK)
4. Click the **hammer icon** (build) and then **run** or **debug**

Alternatively, open `*.ioc` in STM32CubeMX to reconfigure peripherals.

## 📸 CONNECTION

<img width="900" height="510" alt="diagram_stm32_projekt drawio" src="https://github.com/user-attachments/assets/0c47e43f-3942-4c48-aa71-02d4264de41b" />
<img width="752" height="700" alt="image" src="https://github.com/user-attachments/assets/40eb4664-be18-44ba-b571-4b0996270776" />


