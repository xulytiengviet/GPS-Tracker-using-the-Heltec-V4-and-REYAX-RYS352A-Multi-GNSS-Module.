# 🛰️ GPS Tracker using Heltec WiFi LoRa 32 V4 and REYAX RYS352A GNSS Module

![Complete Setup](https://playwithcircuit.com/wp-content/uploads/2026/06/GPS-Tracker-using-Reyax-GNSS-Module-and-Heltec-V4-Board.webp)

A browser-based **real-time GPS Tracker** built using the **Heltec WiFi LoRa 32 V4** development board and the **REYAX RYS352A Multi-GNSS Module**.

The project receives location data from multiple GNSS satellite constellations, displays the current coordinates on the onboard OLED display, and visualizes the current location and travelled path on an interactive OpenStreetMap interface—all without relying on any third-party cloud platform.

> 📖 **Complete Tutorial:** https://playwithcircuit.com/gps-tracker-using-reyax-gnss-module-and-heltec-v4-board/

---

## ✨ Features

* 🛰️ Multi-GNSS support (GPS, GLONASS, Galileo, BeiDou & QZSS)
* 🌍 Browser-based live location tracking using OpenStreetMap
* 🚗 Displays travelled path in real time
* 📍 Shows latitude, longitude, and satellite count on the onboard OLED
* ☁️ No cloud platform or account registration required
* 📡 Built-in web server hosted directly on the Heltec board
* 🔌 Minimal hardware connections using the dedicated GNSS interface
* 📱 Accessible from any smartphone, tablet, or computer connected to the same Wi-Fi network
* 🚀 Beginner-friendly and easy to customize

---

## 🛠 Hardware Required

| Component                 |   Quantity  |
| ------------------------- | :---------: |
| Heltec WiFi LoRa 32 V4    |      1      |
| REYAX RYS352A GNSS Module |      1      |
| Breadboard                |      1      |
| Breadboard Power Supply   |      1      |
| 12V Lithium Battery       |      1      |
| USB Type-C Cable          |      1      |
| Jumper Wires              | As required |

---

## 💻 Software Requirements

* Arduino IDE 2.3.9 or later
* Heltec ESP32 Dev-Boards Library
* TinyGPS++
* ESP32 WiFi Library
* ESP32 WebServer Library

---

## 🔌 Hardware Connections

![Hardware](https://playwithcircuit.com/wp-content/uploads/2026/06/GPS-Tracker-Circuit-Diagram.webp)

| RYS352A GNSS Module | Heltec WiFi LoRa 32 V4 |
| ------------------- | ---------------------- |
| TX                  | GPIO38 (RX)            |
| VCC                 | 3.3V                   |
| GND                 | GND                    |

> **Note:** The GNSS module can also be connected directly to the dedicated Molex GNSS connector available on the Heltec board.

---

## ⚙️ How It Works

1. The RYS352A GNSS module receives signals from multiple navigation satellite constellations.
2. The Heltec board reads the NMEA data through UART communication.
3. Latitude, longitude, and satellite count are extracted from the received data.
4. The current coordinates are displayed on the onboard OLED display.
5. The board connects to a Wi-Fi hotspot and starts an embedded web server.
6. Opening the assigned IP address in a browser displays the current location on an interactive OpenStreetMap.
7. As the tracker moves, the marker position updates automatically and the travelled path is drawn in real time.

![Hardware](https://playwithcircuit.com/wp-content/uploads/2026/06/GPS-Tracker-Prototype.jpg)


---

## 📷 Expected Output

* OLED displays:

  * Latitude
  * Longitude
  * Connected satellites

* Browser displays:

  * Live GPS location
  * Interactive OpenStreetMap
  * Real-time travelled path

---

## 🚗 Applications

* Vehicle Tracking
* Fleet Monitoring
* Asset Tracking
* Personal Safety Devices
* IoT Location Monitoring
* Outdoor Navigation Systems

---

## 📖 Complete Tutorial

A detailed tutorial including:

* Hardware overview
* Circuit diagram
* Complete source code
* Code explanation
* GNSS working principle
* Testing
* Troubleshooting

is available on **PlayWithCircuit**

---

## 🌐 About PlayWithCircuit

**PlayWithCircuit** publishes practical electronics, Arduino, ESP32, IoT, and embedded systems tutorials designed for students, hobbyists, makers, and professional engineers.

---

## ⭐ Support

If you found this project helpful:

* ⭐ Star this repository
* 🍴 Fork it
* 📢 Share it with the maker community
* 🌍 Visit **Play with Circuit** for more embedded systems projects

