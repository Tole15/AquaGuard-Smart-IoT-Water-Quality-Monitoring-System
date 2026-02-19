# 🌊 AquaGuard
### Engineering Portfolio Project – Embedded Systems & IoT

---

## 📌 Project Summary

**AquaGuard** is a full-stack embedded IoT system for real-time water quality monitoring designed for residential and commercial applications.

The project integrates:

- Embedded firmware development (ATmega328P)
- Wireless communication (ESP32 – WiFi)
- Backend API + Database integration
- Mobile application (Flutter)
- Sensor calibration & statistical validation

This project demonstrates practical competencies in:

- Embedded C development
- ADC signal processing
- Serial communication protocols
- IoT architecture design
- Statistical modeling & calibration
- System integration & validation

---

## 🎯 Engineering Objectives

- Design a reliable multi-sensor acquisition system
- Implement real-time embedded processing
- Develop wireless data transmission pipeline
- Build end-to-end IoT infrastructure
- Validate measurement accuracy statistically
- Optimize sampling & system stability

---

## 🧠 System Architecture

```
Sensors → ATmega328P → UART → ESP32 → WiFi → REST API → Database → Mobile App
```

### Embedded Layer (ATmega328P)

Responsibilities:

- 10-bit ADC acquisition
- Sensor signal conditioning
- Polynomial & linear calibration models
- 1-Wire protocol implementation (DS18B20)
- LCD interface (HD44780)
- UART formatted data transmission

Technologies:

- Embedded C
- Interrupt-driven timing
- Fixed-point data handling
- Serial protocol framing

---

## 🔬 Sensors & Measurement Engineering

| Parameter | Sensor | Interface | Engineering Consideration |
|------------|--------|-----------|----------------------------|
| pH | E201-BNC | Analog | Dual-slope calibration model |
| Turbidity | SEN0189 | Analog | Polynomial regression fit |
| Temperature | DS18B20 | 1-Wire | Digital CRC verification |

### Calibration Approach

- MATLAB regression modeling
- Error minimization techniques
- 95% confidence interval validation
- Hypothesis testing using t-distribution

Results:

- Turbidity error reduced from ~10% to ~2%
- pH variance σ ≈ 0.042
- Stable repeated measurements over 100+ samples

---

## 📡 Communication Engineering

### UART (MCU ↔ ESP32)

- 9600 bps
- 8N1 configuration
- Structured ASCII frame
- Voltage level adaptation (5V → 3.3V divider)

### Wireless Layer (ESP32)

- IEEE 802.11
- HTTP POST requests
- JSON payload structure
- Retry & timeout handling

---

## 🖥 Backend & Application Layer

### REST API

- Data ingestion endpoint
- MySQL database storage
- Structured query retrieval

### Flutter Application

- Real-time dashboard
- Historical trend visualization
- User authentication
- API integration

---

## ⚙️ Sampling & System Optimization

| Sensor | Interval | Rationale |
|----------|----------|-----------|
| Turbidity | 5 s | Rapid contamination detection |
| Temperature | 10 s | Slow dynamic response |
| pH | 30 s | Chemical stabilization time |

Design optimizations:

- ADC noise reduction
- Decoupling capacitors on AREF
- 1-Wire pull-up resistor sizing
- Structured firmware state machine

---

## 📊 Validation & Testing

Verified:

✔ Measurement repeatability  
✔ Calibration consistency  
✔ Serial data integrity  
✔ WiFi transmission reliability  
✔ API response correctness  
✔ Mobile data visualization  

Testing methodology:

- 100+ measurement samples
- Statistical variance analysis
- Controlled test environment comparison

---

## 💰 Engineering Economics

| Metric | Value |
|--------|-------|
| Production Cost | ~$1,180 MXN |
| Proposed Price | ~$2,500 MXN |
| Contribution Margin | ~$1,320 MXN |
| Break-even Estimate | ~76 units/month |

---

## 🧩 Engineering Challenges Solved

- Sensor noise & drift compensation
- Mixed-voltage serial communication
- WiFi reconnection robustness
- Data parsing synchronization
- Embedded memory constraints

---

## 🛠 Technical Skill Demonstration

This project demonstrates competency in:

- Embedded firmware design
- Analog-to-digital conversion
- Calibration algorithms
- Serial communication protocols
- IoT system architecture
- Statistical data validation
- Mobile & backend integration
- Engineering cost analysis

---

## 🚀 Potential Industry Applications

- Smart pool monitoring systems
- Industrial water quality supervision
- Residential IoT water safety
- Remote environmental monitoring

---


## 👨‍💻 Author

César Alejandro Tolentino Mendoza  
Engineering Student – Embedded Systems & Telecommunications  
Universidad de Colima

---

---

## 📎 Annexes & Supporting Documentation

Additional technical material, raw data, extended reports, source files, and supplementary validation documents are available at:

🔗 **Google Drive – AquaGuard Annexes:**  
https://drive.google.com/drive/folders/1Fcnc_8k_u9e0-7VuJrHMsaWL_Mp7221n?usp=sharing

Contents include:

- Extended engineering report
- Calibration datasets
- Statistical validation analysis
- Experimental test evidence
- Additional schematics and documentation

---

## 📜 License

Academic and prototyping purposes only.
