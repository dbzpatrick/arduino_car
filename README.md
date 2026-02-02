# Plan B

## 注
- need to install AccelStepper (by Mike McCauley) from Arduino library

## components used
- 2× 28BYJ-48 5V Stepper Motors
- 2× ULN2003 Stepper Driver Boards
- MB102 YwRobot Power Supply Module
- 2× 16340 Li-ion batteries (2S pack)
- HC-SR04 Ultrasonic Distance Sensors

# wiring plan

### Battery → MB102 Input
| From | To |
|------|----|
| Battery + (2S) | MB102 VIN+ / DC Jack + |
| Battery − (2S) | MB102 GND |

### Set MB102
- Set output jumper to **5V** (not 3.3V).

---

## ⚡ MB102 5V Output → Devices

### MB102 → Arduino Uno
| MB102 | Arduino |
|--------|---------|
| +5V | 5V |
| GND | GND |

> Unplug MB102 or USB when uploading code to avoid backfeed.

---

### MB102 → ULN2003 (both boards)
| ULN2003 | MB102 |
|----------|--------|
| VCC | +5V |
| GND | GND |

---

### MB102 → HC-SR04
| HC-SR04 | MB102 |
|---------|--------|
| VCC | +5V |
| GND | GND |

---

## 🧭 ULN2003 Signal Wiring

### Left Motor (ULN2003 #1 → Arduino)
| ULN2003 | Arduino |
|----------|---------|
| IN1 | D2 |
| IN2 | D3 |
| IN3 | D4 |
| IN4 | D5 |

### Right Motor (ULN2003 #2 → Arduino)
| ULN2003 | Arduino |
|----------|---------|
| IN1 | D6 |
| IN2 | D7 |
| IN3 | D10 |
| IN4 | D11 |

---

## 📡 HC-SR04 Signal Wiring

| HC-SR04 | Arduino |
|---------|---------|
| TRIG | D9 |
| ECHO | D8 |

---

## 🔧 Motor Connections
- Plug each **28BYJ-48** into its ULN2003 board’s **5-pin socket**.
- Do not wire stepper motors directly to Arduino.

---

## Pre-Power Checklist
- [ ] Battery is **2 cells in series (2S)** only  
- [ ] MB102 input connected to 2S battery  
- [ ] MB102 output set to **5V**  
- [ ] Arduino powered from MB102 5V or USB  
- [ ] Both ULN2003 boards powered from MB102 5V  
- [ ] HC-SR04 powered from MB102 5V  
- [ ] All grounds are common  



# ignore below

# Wiring Guide

## 1. SA8302 → Arduino Uno

| SA8302 | Arduino |
|--------|---------|
| PWMA   | D5 |
| AIN1   | D2 |
| AIN2   | D3 |
| PWMB   | D6 |
| BIN1   | D4 |
| BIN2   | D7 |
| STBY   | 5V |
| VCC    | 5V |
| GND    | GND |
| VM     | Battery + |

---

## 2. Motors → SA8302

| Motor | SA8302 |
|--------|---------|
| Left motor  | AO1, AO2 |
| Right motor | BO1, BO2 |

> If a motor spins the wrong way, swap its two wires.

---

## 3. HC-SR04 → Arduino

| HC-SR04 | Arduino |
|---------|---------|
| VCC | 5V |
| GND | GND |
| TRIG | D9 |
| ECHO | D8 |
