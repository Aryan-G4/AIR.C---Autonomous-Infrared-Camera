# AIDA: Automated Infrared Detection Aperture

A self-rotating, fire-detecting robot that uses infrared imaging and wireless communication to identify and alert the presence of fire in real time.  
This project combines mechanical motion, embedded firmware, and networked communication into a compact autonomous system.

---

## Overview

The robot continuously rotates using a stepper motor while scanning its surroundings with an infrared camera module.  
When a fire source is detected, it:
1. Stops rotating.
2. Sends a wireless alert to another ESP32 device using the ESP-NOW protocol.

This system demonstrates a practical integration of computer vision, sensor fusion, and real-time control on embedded hardware.

---

## Features

- 360° autonomous rotation via stepper motor control  
- Real-time fire detection using an **MLX90640 infrared camera**  
- Wireless communication via **ESP-NOW** between two ESP32 modules  
- Powered by a **5V portable phone charger** for mobility  
- Automatic halt and alert transmission upon fire detection  
- Modular hardware architecture for easy debugging and extension  

---
## Solidworks model (3D printed)
<img width="894" height="1093" alt="image" src="https://github.com/user-attachments/assets/1a46ad9c-f759-4687-a3e5-eea4655ce61a" />
<img width="1192" height="1161" alt="image" src="https://github.com/user-attachments/assets/bb2ac3ba-a122-4a52-987d-482a2ab05275" />

## Physical Model
<img width="915" height="1217" alt="image" src="https://github.com/user-attachments/assets/da08b78b-706a-4cf4-9826-1c3fa212ee07" />
<img width="735" height="1210" alt="image" src="https://github.com/user-attachments/assets/8a4446b0-f886-4420-b914-4266cbb61a39" />
https://github.com/user-attachments/assets/6a3793b7-10fc-445e-afd6-0ba73308c616


## Hardware Components

| Component | Description | Purpose |
|------------|--------------|----------|
| **ESP32 DevKit V1** | Wi-Fi + Bluetooth microcontroller | Main processing and communication unit |
| **MLX90640 IR Camera** | 32×24 thermal imaging sensor | Detects fire via infrared radiation |
| **TP339 Stepper Motor Driver** | Stepper control interface | Controls swivel rotation of the robot |
| **Stepper Motor (NEMA 17)** | Bipolar motor | Rotates the robot base |
| **5V Portable Power Bank** | External USB supply | Powers the ESP32 and peripherals |
| **Custom Mount and Chassis** | 3D-printed or machined structure | Holds the electronics and provides rotation support |

---

## System Architecture

```text
+-------------------+
|   MLX90640 IR Cam |
+-------------------+
           |
           v
+-------------------+       ESP-NOW        +-------------------+
|   ESP32 DevKit V1 |  <-----------------> |   Remote ESP32 Rx |
|   (Main Robot)    |                     | (Alert Receiver)   |
+-------------------+                     +-------------------+
           |
           v
+-------------------+
|  TP339 Driver IC  |
+-------------------+
           |
           v
+-------------------+
|   Stepper Motor   |
+-------------------+
           |
           v
     [Robot Base]
