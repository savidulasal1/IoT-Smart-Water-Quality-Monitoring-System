# 🐟 IoT-Based Fish Tank Water Quality Monitoring System

## Overview

The **IoT-Based Fish Tank Water Quality Monitoring System** is designed to continuously monitor the water quality of a fish tank using an **ESP32 microcontroller** and an **Android application**. The system measures key water quality parameters in real time and notifies users when the water conditions become unsafe for fish.

## Features

* Real-time monitoring of:

  * pH
  * Temperature
  * Turbidity
  * Total Dissolved Solids (TDS)
* Wi-Fi communication using ESP32
* Cloud data storage with Firebase Realtime Database
* Android application for real-time monitoring
* Automatic alerts when water quality exceeds safe limits
* Recommended corrective actions displayed in the mobile application

## System Architecture

```
Water Sensors
     │
     ▼
+----------------+
|     ESP32      |
+----------------+
     │ Wi-Fi
     ▼
+-------------------------+
| Firebase Realtime DB    |
+-------------------------+
     │
     ▼
+-------------------------+
| Android Application     |
| • View live data        |
| • Receive alerts        |
| • View recommendations  |
+-------------------------+
```

## Hardware Components

* ESP32 Development Board
* pH Sensor
* DS18B20 Temperature Sensor
* Turbidity Sensor
* TDS Sensor
* 12V Power Supply
* Buck Converter (12V to 5V)

## Software and Tools

* Arduino IDE
* Android Studio
* Firebase Realtime Database
* Java (Android)
* C++ (ESP32 Firmware)

## Water Quality Parameters

| Parameter   | Safe Range    |
| ----------- | ------------- |
| pH          | 6.5 – 7.5     |
| Temperature | 24 – 27°C     |
| Turbidity   | Below 25 NTU  |
| TDS         | 150 – 300 ppm |

## Sensor Calibration

To improve measurement accuracy, each sensor was calibrated using a standard reference instrument:

* pH Sensor → pH Meter
* Temperature Sensor → Thermometer
* Turbidity Sensor → Turbidity Meter
* TDS Sensor → TDS Meter

The corresponding ESP32 ADC values were recorded and compared with the reference measurements to establish accurate calibration relationships.

## Project Workflow

1. Sensors measure water quality parameters.
2. ESP32 reads the sensor values.
3. Data is transmitted to Firebase via Wi-Fi.
4. Firebase stores and synchronizes the data.
5. The Android application displays real-time readings.
6. Alerts and recommendations are generated when any parameter exceeds the safe range.

## Repository Structure

```
├── ESP32_Code/
├── Android_App/
├── Documentation/
├── Images/
└── README.md
```

## Future Improvements

* Automatic water pump control
* Automatic pH adjustment
* Historical data visualization
* Machine learning for water quality prediction
* Support for multiple fish tanks
* iOS application

## Authors

Savidu Lasal
