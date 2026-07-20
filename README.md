# BlueLine One

BlueLine One is a custom ESP32-S3 round AMOLED CAN bus gauge. It displays live OBD-II vehicle data including RPM, speed, throttle, intake air temperature, coolant temperature, battery voltage, airflow, MPG, TPMS UI, and a chronograph-style display with touch controls.

This project is designed around the Waveshare ESP32-S3 1.75" round AMOLED display and a LeLink^2 Bluetooth OBD-II adapter.

---

## Current Features

- 4-in-1 All-In-One gauge
  - RPM shift light
  - Speed
  - Throttle
  - Intake air temperature
- Individual gauge screens
  - RPM
  - Speed
  - Throttle
  - Intake air temperature
  - Coolant temperature
  - Battery voltage
  - TPMS UI
  - Airflow / MAF
  - MPG
  - Chronograph / tachymetre-style display
- BLE OBD-II support
- Touch swipe navigation
- Long-press controls
- Adjustable ring colors
- No-color / ring-off mode
- PWR button chronograph start/stop
- BOOT button chronograph reset

---

## Hardware Used

- Waveshare ESP32-S3 1.75" AMOLED round display, 466×466
- BLE OBD-II adapter, such as LELink²
- USB-C power source
- Optional 3D printed mount or enclosure

---

## Required Arduino Libraries

Install these from the Arduino IDE Library Manager:

- `lvgl`
- `GFX Library for Arduino`
- `SensorLib`
- `XPowersLib`

Also install the ESP32 board package by Espressif.

---

## Arduino IDE Board Settings

Recommended settings:

```text
Board: ESP32S3 Dev Module
USB CDC On Boot: Enabled
Flash Size: 16MB
PSRAM: OPI PSRAM
Partition Scheme: Huge APP
Upload Speed: 115200
