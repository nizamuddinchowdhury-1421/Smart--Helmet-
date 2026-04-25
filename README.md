# 🪖 Smart Helmet – Arduino-Based Safety System

![Arduino](https://img.shields.io/badge/Platform-Arduino%20Uno%20R3-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)

> An embedded microcontroller-based smart helmet system designed to enhance rider safety through real-time detection of helmet non-compliance, alcohol intoxication, and driver drowsiness — with automatic vehicle control via RF communication.

---

## 📌 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Hardware Components](#hardware-components)
- [Circuit Diagram](#circuit-diagram)
- [Module Descriptions](#module-descriptions)
- [How It Works](#how-it-works)
- [Installation & Setup](#installation--setup)
- [Code Structure](#code-structure)
- [LCD Display Output](#lcd-display-output)
- [Contributing](#contributing)
- [License](#license)

---

## 🧠 Overview

The **Smart Helmet** is a microprocessor-based safety project built around the **Arduino Uno R3**. It integrates multiple sensors and wireless communication to monitor the rider's condition in real time. If any safety violation is detected — such as not wearing the helmet, alcohol consumption, or drowsiness — the system alerts the rider via buzzer and LEDs, and can automatically cut the vehicle's engine through an RF-controlled relay at the vehicle unit.

---

## ✅ Features

| Feature | Description |
|---|---|
| 🪖 Helmet Detection | Detects whether the helmet is worn before allowing ignition |
| 🍺 Alcohol Detection | Monitors breath alcohol level using MQ-3/MQ-135 gas sensor |
| 😴 Drowsiness Detection | Detects eye closure or head tilt indicating driver fatigue |
| 📡 RF Communication | Transmits safety status wirelessly to the vehicle receiver unit |
| ⚙️ Auto Engine Cut-off | I2C relay disables BO motor (engine) upon violation detection |
| 📟 LCD Display | Real-time status messages shown on 16×2 I2C LCD |
| 🔔 Audio/Visual Alert | Buzzer and LEDs warn the rider of any detected hazard |

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    HELMET UNIT                          │
│                                                         │
│  [Helmet Sensor] ──┐                                    │
│  [Alcohol Sensor]──┤──► Arduino Uno R3 ──► RF TX ──────►│──► (RF Signal)
│  [Drowsy Sensor] ──┘        │                           │
│                             ├──► LCD Display (I2C)      │
│                             ├──► Buzzer                 │
│                             └──► LEDs (Status)          │
└─────────────────────────────────────────────────────────┘
                                        │
                                  (RF 433 MHz)
                                        │
                                        ▼
┌─────────────────────────────────────────────────────────┐
│                   VEHICLE UNIT                          │
│                                                         │
│  RF RX ──► Arduino Uno R3 ──► I2C Relay ──► BO Motor   │
│                          └──► LED Indicator             │
└─────────────────────────────────────────────────────────┘
```

---

## 🔩 Hardware Components

### Helmet Unit
| Component | Quantity | Purpose |
|---|---|---|
| Arduino Uno R3 | 1 | Main microcontroller |
| MQ-3 / MQ-135 Gas Sensor | 1 | Alcohol detection |
| IR Sensor / Reed Switch | 1 | Helmet worn detection |
| IR Eye Blink Sensor / MPU-6050 | 1 | Drowsiness detection |
| RF Transmitter (433 MHz) | 1 | Wireless data transmission |
| 16×2 LCD Display (I2C) | 1 | Status display |
| Buzzer (Active) | 1 | Audio alert |
| LED (Red, Green, Yellow) | 3 | Visual status indicators |
| Resistors (220Ω, 10kΩ) | As needed | Current limiting |
| 9V / 12V Battery | 1 | Power supply |
| Connecting Wires & Breadboard | — | Circuit connections |

### Vehicle Unit
| Component | Quantity | Purpose |
|---|---|---|
| Arduino Uno R3 | 1 | Receiver microcontroller |
| RF Receiver (433 MHz) | 1 | Wireless data reception |
| I2C Relay Module | 1 | Engine/motor control |
| BO Motor (DC Gear Motor) | 1 | Simulates vehicle engine |
| LED Indicator | 1 | Status indicator |
| 9V / 12V Battery | 1 | Power supply |

---

## 📐 Circuit Diagram

> *(Add your Fritzing or KiCad circuit diagram image here)*

```
Place your circuit diagram image in /assets/circuit_diagram.png
and update this section with: ![Circuit Diagram](assets/circuit_diagram.png)
```

### Pin Mapping — Helmet Unit (Arduino Uno R3)

| Arduino Pin | Connected To |
|---|---|
| A0 | MQ-3 Alcohol Sensor (Analog Out) |
| D2 | IR Helmet Detection Sensor |
| D3 | IR Eye Blink / Drowsiness Sensor |
| D4 | RF Transmitter Data Pin |
| D8 | Buzzer |
| D9 | Red LED |
| D10 | Green LED |
| D11 | Yellow LED |
| A4 (SDA) | LCD I2C SDA |
| A5 (SCL) | LCD I2C SCL |
| 5V / GND | Power Rails |

### Pin Mapping — Vehicle Unit (Arduino Uno R3)

| Arduino Pin | Connected To |
|---|---|
| D2 | RF Receiver Data Pin |
| A4 (SDA) | I2C Relay SDA |
| A5 (SCL) | I2C Relay SCL |
| D6 | BO Motor (via Relay) |
| D7 | Status LED |

---

## 📦 Module Descriptions

### 1. 🪖 Helmet Detection
A **reed switch** or **IR proximity sensor** is placed inside the helmet. When the helmet is properly worn, the sensor is triggered (closed/low). If the helmet is not worn, the Arduino prevents the RF transmitter from sending the "safe to start" signal, keeping the vehicle motor off.

### 2. 🍺 Alcohol Detection
The **MQ-3 gas sensor** detects ethanol vapor in the rider's breath. The analog output is read by the Arduino and compared against a calibrated threshold. If the alcohol level exceeds the limit:
- Buzzer activates
- Red LED turns ON
- LCD displays `"Alcohol Detected!"`
- RF transmitter sends an engine cut-off command

### 3. 😴 Drowsiness Detection
An **IR eye blink sensor** or **MPU-6050 accelerometer/gyroscope** is used to detect drowsiness:
- **IR Eye Sensor**: Detects prolonged eye closure
- **MPU-6050**: Detects head nodding or tilting beyond a threshold angle

Upon drowsiness detection:
- Buzzer sounds an alert
- Yellow LED flashes
- LCD shows `"Drowsy! Wake Up!"`
- Engine cut-off signal sent via RF

### 4. 📡 RF Transmitter/Receiver (433 MHz)
The helmet unit uses an **RF transmitter** to wirelessly send safety status codes to the vehicle unit's **RF receiver**. The `VirtualWire` or `RadioHead` library handles the encoding and decoding of data packets.

### 5. ⚙️ I2C Relay & BO Motor
The vehicle unit's Arduino receives the RF signal and controls an **I2C relay module** which in turn enables or disables the **BO motor** (representing the vehicle engine). The I2C interface reduces wiring complexity.

### 6. 📟 LCD Display
A **16×2 I2C LCD** on the helmet unit displays real-time status:
- System boot message
- Current detection status
- Warnings and alerts

---

## ⚙️ How It Works

```
START
  │
  ▼
Is Helmet Worn?
  ├── NO  ──► Buzzer ON + Red LED + LCD "Wear Helmet!" ──► Motor OFF
  └── YES
        │
        ▼
   Alcohol Detected?
        ├── YES ──► Buzzer ON + LCD "Alcohol!" ──► RF TX (Cut-off) ──► Motor OFF
        └── NO
              │
              ▼
         Drowsiness Detected?
              ├── YES ──► Buzzer ON + LCD "Drowsy!" ──► RF TX (Cut-off) ──► Motor OFF
              └── NO
                    │
                    ▼
               All Clear ──► Green LED + LCD "Safe to Ride" ──► RF TX (Allow) ──► Motor ON
```

---

## 🛠️ Installation & Setup

### Prerequisites
- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8+ or v2.x)
- Required Libraries:
  - `LiquidCrystal_I2C` — for LCD display
  - `Wire` — for I2C communication
  - `VirtualWire` or `RadioHead` — for RF communication
  - `MPU6050` *(if using gyroscope for drowsiness)*

### Installing Libraries
1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search and install:
   - `LiquidCrystal I2C` by Frank de Brabander
   - `RadioHead` by Mike McCauley
   - `MPU6050` by Electronic Cats *(optional)*

### Uploading the Code

**Helmet Unit:**
```bash
1. Open /helmet_unit/helmet_unit.ino in Arduino IDE
2. Select Board: Arduino Uno
3. Select correct COM Port
4. Click Upload
```

**Vehicle Unit:**
```bash
1. Open /vehicle_unit/vehicle_unit.ino in Arduino IDE
2. Select Board: Arduino Uno
3. Select correct COM Port
4. Click Upload
```

---

## 📁 Code Structure

```
smart-helmet/
│
├── helmet_unit/
│   └── helmet_unit.ino          # Main code for helmet Arduino
│
├── vehicle_unit/
│   └── vehicle_unit.ino         # Main code for vehicle Arduino
│
├── assets/
│   ├── circuit_diagram.png      # Full circuit diagram
│   ├── block_diagram.png        # System block diagram
│   └── demo.gif                 # Project demo
│
├── docs/
│   └── project_report.pdf       # Full project documentation
│
└── README.md
```

---

## 📟 LCD Display Output

| Situation | LCD Message |
|---|---|
| System Boot | `Smart Helmet v1.0` |
| Helmet Not Worn | `Wear Your Helmet!` |
| Alcohol Detected | `Alcohol Detected!` |
| Drowsiness Detected | `Drowsy! Wake Up!` |
| All Clear | `Safe to Ride :)` |
| Engine Cut by RF | `Engine Disabled` |

---

## 🚀 Future Improvements

- GPS module for real-time location tracking
- GSM/SIM800L for SMS alert to emergency contacts
- OLED display upgrade for better visibility
- Mobile app integration via Bluetooth (HC-05)
- Data logging to SD card for accident analysis
- Solar-assisted battery charging

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome!

1. Fork the repository
2. Create your feature branch: `git checkout -b feature/new-sensor`
3. Commit your changes: `git commit -m 'Add new sensor module'`
4. Push to the branch: `git push origin feature/new-sensor`
5. Open a Pull Request

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

---

## 👨‍💻 Author

> Built with ❤️ for road safety.  
> Feel free to ⭐ this repo if you found it useful!

---

*"Because every ride should end safely."* 🏍️
