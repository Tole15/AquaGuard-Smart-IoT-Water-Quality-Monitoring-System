# 🌊 AquaGuard

### Smart IoT Water Quality Monitoring System

![Platform](https://img.shields.io/badge/MCU-ATmega328P-blue)
![Connectivity](https://img.shields.io/badge/WiFi-ESP32-green)
![Sensors](https://img.shields.io/badge/Sensors-pH%20%7C%20Turbidity%20%7C%20Temp-orange)
![App](https://img.shields.io/badge/App-Flutter-blueviolet)
![Cost](https://img.shields.io/badge/Cost-~1180MXN-critical)

---

## 📌 Overview

**AquaGuard** is an embedded + IoT system designed for real-time monitoring of water quality in:

- 🏊 Swimming pools
- 🏠 Residential water tanks (tinacos)
- 🏨 Hotels & recreational centers

The system continuously measures:

- **pH**
- **Turbidity (NTU)**
- **Temperature (°C)**

Data is processed locally and transmitted via Wi-Fi to a server and mobile application.

---

## 🧠 System Architecture

```
Sensors → ATmega328P → UART → ESP32 → Wi-Fi → Server/API → Flutter App
```

### 🔹 Data Flow

1. Sensors capture analog/digital signals.
2. **ATmega328P**:
   - Reads ADC channels (pH & turbidity)
   - Applies calibration models
   - Reads DS18B20 via 1-Wire
   - Displays data on LCD
   - Sends formatted string via UART
3. **ESP32**:
   - Parses incoming data
   - Connects to Wi-Fi
   - Sends JSON via HTTP POST
4. **Server**:
   - Stores data (MySQL)
   - Provides REST API
5. **Flutter App**:
   - Displays real-time dashboard
   - Shows historical graphs
   - Sends alerts

---

## 🔬 Sensors & Measurement

| Parameter | Sensor | Range | Precision |
|------------|--------|--------|-----------|
| pH | E201-BNC | 0–14 | ±0.1 pH |
| Turbidity | SEN0189 | 0–1000 NTU | ±5% FS |
| Temperature | DS18B20 | -55°C to 125°C | ±0.5°C |

---

## 📊 Calibration & Statistical Validation

### 🔹 Turbidity

- Polynomial regression (MATLAB)
- Error reduced from **10% → 2%**
- Statistical error minimization

### 🔹 pH

- Dual linear model:
  - Acid region (pH < 7)
  - Basic region (pH ≥ 7)
- Regression-based slope & intercept calibration

### 🔹 Confidence Testing

- ≥100 samples collected
- 95% confidence level
- t-distribution hypothesis testing
- Low variance and consistent measurements

---

## 📡 Communication Protocols

### UART (ATmega ↔ ESP32)

- 9600 bps
- 8N1 configuration
- NRZ encoding

### Wi-Fi (ESP32 ↔ Server)

- IEEE 802.11
- OFDM modulation
- QPSK / QAM schemes
- HTTP + JSON data format

---

## 🖥 Mobile Application (Flutter)

Features:

- Real-time dashboard
- Historical data visualization
- Push notifications
- User authentication
- REST API integration

Implemented screens:

- Login
- SignUp
- Dashboard

---

## ⚙️ Sampling Strategy

| Sensor | Sampling Interval |
|----------|------------------|
| Turbidity | 5 seconds |
| Temperature | 10 seconds |
| pH | 30 seconds |

Optimized for:

- Stability
- Energy efficiency
- Noise reduction
- Meaningful data acquisition

---

## 💰 Cost Analysis

**Estimated unit production cost:**

```
≈ $1,180 MXN
```

**Proposed selling price:**

```
$2,500 MXN
```

**Contribution margin:**

```
$1,320 MXN per unit
```

**Break-even point:**

```
≈ 76 units / month
```

---

## 🔒 Engineering Considerations

- 10-bit ADC with AVCC reference filtering
- 5V → 3.3V logic level adaptation (UART divider)
- 1-Wire pull-up resistor (4.7kΩ)
- LCD HD44780 timing compliance
- Error handling & retry logic in Wi-Fi transmission
- Data validation before server upload

---

## 📈 System Validation

Validated components:

✔ Sensor accuracy  
✔ Calibration model stability  
✔ UART communication integrity  
✔ Wi-Fi transmission reliability  
✔ REST API functionality  
✔ Mobile visualization  
✔ Error handling mechanisms  

---

## 🚀 Future Improvements

- Chlorine sensor integration
- Electrical conductivity measurement
- Solar-powered version
- MQTT integration
- End-to-end encryption
- Industrial PCB design
- IP-rated enclosure

---

## 📂 Repository Structure

```
├── firmware/
│   ├── atmega328p/
│   └── esp32/
├── mobile_app/
│   └── flutter_app/
├── server/
│   ├── api/
│   └── database/
├── hardware/
│   ├── schematics/
│   └── pcb/
└── docs/
    └── ReporteAquaguard5C.pdf
```

---

## 📜 License

This project is provided for academic, research, and prototyping purposes.
