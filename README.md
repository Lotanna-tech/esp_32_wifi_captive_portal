# 🚀 ESP32 Wi-Fi Captive Portal

**Turn your ESP32 into a smart Wi-Fi setup hero!**  

Imagine powering up your ESP32 and instantly having a **friendly web portal** pop up where you can configure Wi-Fi—no serial monitor, no hard-coded credentials. This project does exactly that. Perfect for IoT devices, smart home projects, or just showing off your embedded skills!  

---

## 🌟 Features
- **Soft AP Mode:** The ESP32 creates its own Wi-Fi network.  
- **Captive Portal Web UI:** Easy-to-use browser interface to enter your Wi-Fi SSID and password.  
- **Persistent Storage:** Credentials are saved in NVS, so you don’t have to re-enter them every time.  
- **Automatic Fallback:** If Wi-Fi fails, the ESP32 reverts to AP mode, keeping you in control.  
- **Sleek UI:** Responsive HTML/CSS with password visibility toggle for convenience.  
- **Portfolio-Ready:** Neat, fully functional, and perfect to show off your embedded systems skills.  

---

## 🛠 Hardware
- **ESP32 Dev Board** (any variant)  
- Optional: OLED, sensors, or other peripherals for extended projects  

---

## 💻 Software
- **ESP-IDF v5.x**  
- **FreeRTOS** (built into ESP-IDF)  
- **HTTP Server Component** (`esp_http_server`)  
- **NVS** for storing Wi-Fi credentials securely  

---

## ⚡ How to Use
1. Flash the firmware to your ESP32:  
   ```bash
   idf.py flash monitor
