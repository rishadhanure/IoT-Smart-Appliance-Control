# IoT-Based Smart Appliance Control

## Overview

This project is an **IoT-based smart appliance control system** using the **ESP8266 NodeMCU**. It monitors temperature and ambient light intensity to automatically control a fan and a light.

The system also provides a **web-based dashboard** for real-time monitoring and manual appliance control over a local Wi-Fi network.

## Objectives

- Develop an IoT-based appliance control system using ESP8266.
- Monitor temperature and ambient light intensity.
- Automatically control a fan based on temperature.
- Automatically control a light based on ambient light conditions.
- Enable remote monitoring and manual appliance control through a web dashboard.

## Components Used

- ESP8266 NodeMCU
- DS18B20 Temperature Sensor
- LDR (Light Dependent Resistor)
- I2C LCD 16×2 Display
- Relay Modules
- Fan
- Light
- Connecting Wires
- Supporting Components

## Working Principle

The ESP8266 continuously reads temperature data from the **DS18B20 temperature sensor** and ambient light intensity from the **LDR**.

The system operates in two modes:

### 1. Automatic Mode

In automatic mode, the appliances are controlled automatically according to sensor readings.

#### Fan Control

- Fan turns **ON** when the temperature exceeds **40°C**.
- Fan turns **OFF** when the temperature is **40°C or below**.

#### Light Control

- Light turns **ON** when the LDR reading falls below the configured threshold of **400**.
- Light turns **OFF** when sufficient ambient light is detected.

### 2. Manual Mode

The system allows users to control the fan and light manually through the web dashboard.

Available controls:

- Fan ON/OFF
- Light ON/OFF
- Automatic mode selection

Selecting an individual appliance control switches the corresponding system operation into **manual mode**.

## Web Dashboard

The ESP8266 hosts a local web server that provides a user-friendly dashboard for monitoring and controlling the connected appliances.

### Dashboard Features

- Real-time temperature monitoring
- Live LDR sensor readings
- Fan ON/OFF control
- Light ON/OFF control
- Automatic mode selection
- Current appliance status

The dashboard refreshes sensor information every **two seconds**.

## Pin Configuration

| Component | ESP8266 Pin |
|---|---|
| DS18B20 Data | D5 |
| Fan Relay | D6 |
| Light Relay | D7 |
| LDR | A0 |
| LCD SDA | D2 |
| LCD SCL | D1 |

## Libraries Required

The following Arduino libraries are required:

- `ESP8266WiFi`
- `ESP8266WebServer`
- `OneWire`
- `DallasTemperature`
- `Wire`
- `LiquidCrystal_I2C`

## Software Requirements

- Arduino IDE
- ESP8266 Board Package
- Required Arduino Libraries
- Web Browser
- Wi-Fi Network

## Setup and Installation

1. Install the **Arduino IDE**.
2. Install the **ESP8266 board package** in Arduino IDE.
3. Install all the required libraries.
4. Connect the components according to the circuit design.
5. Open the project code in Arduino IDE.
6. Enter your **Wi-Fi network name and password** in the code.
7. Select the appropriate **ESP8266 board** and **COM port**.
8. Upload the program to the ESP8266.
9. Open the **Serial Monitor** to find the IP address assigned to the ESP8266.
10. Connect your computer or mobile device to the **same Wi-Fi network**.
11. Enter the ESP8266 IP address in a web browser to access the dashboard.

## Applications

- Smart home automation
- Temperature-based fan control
- Automatic lighting systems
- IoT-based appliance monitoring
- Remote appliance management
- Environment-based appliance control

## Future Improvements

- Add a dedicated manual/automatic mode toggle.
- Introduce temperature hysteresis to reduce frequent relay switching.
- Add appliance scheduling.
- Integrate energy consumption monitoring.
- Improve dashboard security.
- Enable remote access over the internet.
- Add mobile application support.

## Author

**Risha Dhanure**

Electronics and Telecommunication Engineering
