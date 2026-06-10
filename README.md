# 🎯 Firmway-Gunshot — LoRa Gunshot Detection & Alert System

A real-time gunshot detection and alert system for military/border applications using ESP32 and LoRa (SX1278). When a gunshot is detected by the sender node, the receiver instantly displays threat direction, estimated distance, and GPS coordinates on an OLED screen — with vibration and LED alerts.

---

## 📡 How It Works

1. **Sender node** detects a gunshot and transmits a LoRa packet in the format:
   ```
   DETECT:LEFT:420:11.01680:76.95580:10.98765:77.12345
   ```
2. **Receiver node** (this repo) picks up the packet and:
   - Parses direction, distance, observer GPS, and shooter GPS
   - Displays a threat alert on the SSD1306 OLED
   - Triggers vibration motor and LED pulses
   - Logs all data to Serial Monitor

---

## 🛠️ Hardware Required

| Component | Details |
|---|---|
| ESP32 | Main microcontroller |
| RA-02 SX1278 | LoRa module (433 MHz) |
| SSD1306 OLED | 128x64 I2C display |
| Vibration Motor | Via NPN transistor (GPIO 4) |
| LED (optional) | Built-in LED (GPIO 2) |

---

## 🔌 Pin Configuration

**LoRa RA-02 SX1278**

| ESP32 Pin | LoRa Pin |
|---|---|
| GPIO 5 | NSS (CS) |
| GPIO 14 | RST |
| GPIO 2 | DIO0 |
| GPIO 18 | SCK |
| GPIO 19 | MISO |
| GPIO 23 | MOSI |
| 3.3V | VCC |
| GND | GND |

**SSD1306 OLED**

| ESP32 Pin | OLED Pin |
|---|---|
| GPIO 21 | SDA |
| GPIO 22 | SCL |
| 3.3V | VCC |
| GND | GND |

**Vibration Motor**

| ESP32 Pin | Component |
|---|---|
| GPIO 4 | Transistor Base |
| 3.3V / 5V | Motor VCC |
| GND | GND |

**LED**

| ESP32 Pin | Component |
|---|---|
| GPIO 2 | Built-in LED |

> ⚠️ GPIO 2 is shared between LoRa DIO0 and the built-in LED

---

## 📦 Libraries Required

Install via Arduino IDE Library Manager:

- `LoRa` by Sandeep Mistry
- `Adafruit GFX Library`
- `Adafruit SSD1306`

---

## 🚀 Getting Started

1. Clone this repo:
   ```bash
   git clone https://github.com/Sowdeswari26/Firmway-Gunshot.git
   ```
2. Open `Receiver_side.ino` in Arduino IDE
3. Install the required libraries
4. Select **ESP32 Dev Module** as the board
5. Upload and open Serial Monitor at **115200 baud**

> ⚠️ Make sure the LoRa frequency matches the sender (`433E6`)

---

## 📟 OLED Display Output

On threat detection, the OLED shows:
- **THREAT** warning bar
- Direction arrow (LEFT / RIGHT)
- Distance in metres
- Observer GPS coordinates
- RSSI signal strength

---

## 📁 Project Structure

```
Firmway-Gunshot/
├── Receiver_side.ino/
│   └── Receiver_side.ino.ino
└── Sender.ino/
    └── Sender.ino.ino
```

---

## 👤 Author

**Sowdeswari S** — [github.com/Sowdeswari26](https://github.com/Sowdeswari26)

> Built as part of the Stealth Communication System for Military Borders project. 🏆 1st Place — Electrathon'26 National Hackathon.
