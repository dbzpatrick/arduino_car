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
