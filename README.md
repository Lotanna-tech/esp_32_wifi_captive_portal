ESP32 Wi-Fi Captive Portal

Description:
This project implements a Wi-Fi captive portal on the ESP32. It allows users to connect to the ESP32’s soft AP, access a web UI, and input Wi-Fi credentials. Credentials are stored in NVS, and the device can fallback to AP mode if it fails to connect.

Features

Soft AP mode with DHCP server

Captive portal web interface for SSID and password

Credentials saved to NVS for persistent storage

Fallback to AP mode on connection failure

Simple and responsive HTML/CSS UI

Password visibility toggle on the form

Hardware

ESP32 Development Board

Optional: OLED, sensors, or additional modules depending on your project extension

Software

ESP-IDF v5.x

FreeRTOS (built-in with ESP-IDF)

HTTP Server component (esp_http_server)

NVS for storing Wi-Fi credentials

How to Use

Flash the firmware to ESP32:
