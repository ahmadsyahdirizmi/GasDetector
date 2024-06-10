# IoT-Based WC Cleanliness Detection Using MQ-135 Sensor for Ammonia Monitoring

## Overview
This project uses an IoT system to monitor ammonia levels in public restrooms in real-time using the MQ-135 sensor. It aims to improve restroom cleanliness and user experience by providing timely notifications to cleaning staff.

## Problem Statement
Public restrooms often have cleanliness issues due to the inability to monitor odor in real-time, resulting in an unpleasant user experience and difficulty for cleaning staff to prioritize tasks.

## Scope
1. **Gas Detection:** Focus on ammonia (NH3) using MQ-135.
2. **Monitoring Area:** Public restrooms in malls, stations, large offices.
3. **Ammonia Threshold:** 25 ppm as the indicator for cleaning.
4. **IoT Infrastructure:** ESP32 microcontroller with Wi-Fi.
5. **Notifications:** Via Telegram.
6. **Monitoring Frequency:** Real-time, data collected every minute.
7. **System Users:** Cleaning staff only.

## Solution
- **Sensor Setup:** MQ-135 sensors detect ammonia in restrooms.
- **Microcontroller:** ESP32 reads sensor data and sends it to the cloud.
- **IoT Platform:** Data stored and analyzed, triggers notifications via Telegram when ammonia exceeds the threshold.
- **Testing:** System tested and calibrated for accuracy.

## Key Features
1. **Specific Detection:** Accurate ammonia monitoring with MQ-135.
2. **IoT Connectivity:** Real-time data and analysis.
3. **Automatic Notifications:** Alerts cleaning staff automatically.
4. **Efficiency:** Improves restroom cleanliness management.
5. **Health and Comfort:** Ensures a cleaner environment for users.

## Getting Started

### Prerequisites
- ESP32 Microcontroller
- MQ-135 Gas Sensor
- Wi-Fi
- Telegram Account (Bot Father)
- IoT Platform (Blynk)
