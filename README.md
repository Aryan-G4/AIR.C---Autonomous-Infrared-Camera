# AIDA: Automated Infrared Detection Aperture
<img width="400"  alt="image" src="https://github.com/user-attachments/assets/da08b78b-706a-4cf4-9826-1c3fa212ee07" />
<img width="400" alt="image" src="https://github.com/user-attachments/assets/ffaec6b3-e46c-4cc1-a89e-84c2f520a930" />

A self-controlled, fire-detecting robot that uses infrared imaging and wireless communication to identify and alert the presence of fire in real time and wirelessly report it to a host computer.  
This project combines mechanical motion, embedded firmware, and networked communication into a compact autonomous system.
It has been designed with the inspiration of removing multiple traditional ceiling fire detectors, replacing them with one robot to monitor an entire room.
---

## Overview
<img width="2156" height="970" alt="image" src="https://github.com/user-attachments/assets/b1b177dd-06fd-498e-a911-5322ea3c7b52" />

The robot continuously rotates back and forth with a 360 Degree angle using a stepper motor while scanning its surroundings with an infrared camera module.  
When a fire source is detected, it stops rotating and sends a wireless alert to another ESP32 reciever using the ESP-NOW protocol. 
Multiple Fire Monitors can transmit to the reciever 

---

## Features

- 360° autonomous rotation via stepper motor control (NEMA17 + TP339 controller)
- Real-time fire detection using an **MLX90640 infrared camera**  
- Wireless communication via **ESP-NOW** between two ESP32 devkits  
- Powered by a **5V portable phone charger** for mobility  and free rotation, as well as a stepper motor driver powered by a +24V wall adapter
- Automatic halt and alert transmission upon fire detection  
- Modular hardware architecture for easy debugging and extension  

---
## Solidworks model (3D printed)
<img width="400"  alt="image" src="https://github.com/user-attachments/assets/1a46ad9c-f759-4687-a3e5-eea4655ce61a" />
<img width="400"  alt="image" src="https://github.com/user-attachments/assets/bb2ac3ba-a122-4a52-987d-482a2ab05275" />

## Physical Model
<img width="400"  alt="image" src="https://github.com/user-attachments/assets/da08b78b-706a-4cf4-9826-1c3fa212ee07" />
<img width="400"  alt="image" src="https://github.com/user-attachments/assets/8a4446b0-f886-4420-b914-4266cbb61a39" />



## Hardware Components

| Component | Description | Purpose |
|------------|--------------|----------|
| **ESP32 DevKit V1** | Wi-Fi + Bluetooth microcontroller | Main processing and communication unit |
| **MLX90640 IR Camera** | 32×24 thermal imaging sensor | Detects fire via infrared radiation |
| **TP339 Stepper Motor Driver** | Stepper control interface | Controls swivel rotation of the robot |
| **Stepper Motor (NEMA 17)** | Bipolar motor | Rotates the robot base |
| **5V Portable Power Bank** | External USB supply | Powers the ESP32 and all connected low-voltage peripherals |
| **Custom Mount and Chassis** | 3D-printed Strucutre | Holds the electronics and allows the NEMA17 to rotate the camera and devkit to monitor the room |

---


