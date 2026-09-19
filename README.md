# 🛰️ Bộ theo dõi GPS/GNSS thời gian thực — Heltec V4 + REYAX RYS352A

![Mô hình hoàn chỉnh](https://playwithcircuit.com/wp-content/uploads/2026/06/GPS-Tracker-using-Reyax-GNSS-Module-and-Heltec-V4-Board.webp)

Đây là dự án **theo dõi vị trí GPS/GNSS thời gian thực trên trình duyệt**, sử dụng bo mạch **Heltec WiFi LoRa 32 V4** và mô-đun **REYAX RYS352A Multi-GNSS**.

Thiết bị nhận dữ liệu từ nhiều hệ thống vệ tinh GNSS, hiển thị tọa độ hiện tại trên màn hình OLED tích hợp và cung cấp một trang WebGIS dùng **Leaflet + OpenStreetMap** để quan sát vị trí và hành trình trực tiếp — **không cần nền tảng đám mây bên thứ ba**.

> 📖 **Bài hướng dẫn gốc:** https://playwithcircuit.com/gps-tracker-using-reyax-gnss-module-and-heltec-v4-board/

---

## ✨ Tính năng chính

- 🛰️ **Đa hệ GNSS:** GPS, GLONASS, Galileo, BeiDou và QZSS.
- 🗺️ **Bản đồ trực tiếp:** hiển thị vị trí trên OpenStreetMap bằng Leaflet.
- 🚗 **Vẽ hành trình:** cập nhật đường đi theo thời gian thực.
- 📍 **Thông tin tọa độ:** vĩ độ, kinh độ và số vệ tinh.
- 📺 **OLED tích hợp:** hiển thị dữ liệu GNSS ngay trên thiết bị.
- 📡 **Web server nhúng:** ESP32 tự phục vụ giao diện web qua cổng 80.
- ☁️ **Không cần cloud:** không cần tài khoản hay máy chủ trung gian.
- 📱 **Đa thiết bị:** mở bằng điện thoại, máy tính bảng hoặc PC cùng mạng Wi‑Fi.
- 🇻🇳 **Giao diện tiếng Việt:** các nhãn, trạng thái và hướng dẫn được Việt hóa.
- 🎨 **Icon hóa trực quan:** dùng biểu tượng cho vị trí, vệ tinh, Wi‑Fi và hành trình.

---

## 🧰 Phần cứng cần chuẩn bị

| 🔧 Linh kiện | 🔢 Số lượng |
| --- | :---: |
| Heltec WiFi LoRa 32 V4 | 1 |
| REYAX RYS352A GNSS Module | 1 |
| Breadboard | 1 |
| Nguồn cho breadboard | 1 |
| Pin Lithium 12V | 1 |
| Cáp USB Type‑C | 1 |
| Dây jumper | Theo nhu cầu |

---

## 💻 Phần mềm / thư viện

- 🧩 Arduino IDE 2.3.9 hoặc mới hơn
- 📦 Heltec ESP32 Dev-Boards Library
- 🛰️ TinyGPS++
- 📶 ESP32 WiFi Library
- 🌐 ESP32 WebServer Library

---

## 🔌 Sơ đồ kết nối

![Sơ đồ phần cứng](https://playwithcircuit.com/wp-content/uploads/2026/06/GPS-Tracker-Circuit-Diagram.webp)

| 🛰️ RYS352A | 🧠 Heltec WiFi LoRa 32 V4 |
| --- | --- |
| TX | GPIO38 (RX) |
| VCC | 3.3V |
| GND | GND |

> ℹ️ **Lưu ý:** RYS352A cũng có thể kết nối trực tiếp vào đầu nối Molex GNSS chuyên dụng trên bo Heltec V4.

---

## ⚙️ Cách hệ thống hoạt động

1. 🛰️ RYS352A thu tín hiệu từ nhiều chòm vệ tinh định vị.
2. 🔁 Heltec V4 đọc dữ liệu NMEA qua UART.
3. 📍 TinyGPS++ phân tích vĩ độ, kinh độ và số vệ tinh.
4. 📺 Dữ liệu GNSS được hiển thị trên OLED.
5. 📶 ESP32 kết nối vào Wi‑Fi và khởi động web server.
6. 🌐 Trình duyệt truy cập địa chỉ IP của thiết bị.
7. 🗺️ Leaflet hiển thị vị trí trên OpenStreetMap.
8. 🚗 Khi thiết bị di chuyển, marker và đường hành trình được cập nhật tự động.

![Mẫu thử phần cứng](https://playwithcircuit.com/wp-content/uploads/2026/06/GPS-Tracker-Prototype.jpg)

---

## 🔐 Cấu hình Wi‑Fi trước khi nạp code

Trong tệp `gnss.ino`, sửa hai dòng sau:

```cpp
char ssid[] = "TEN_WIFI";
char pass[] = "MAT_KHAU_WIFI";
```

> 🔒 Không nên đẩy mật khẩu Wi‑Fi thật lên repository công khai.

---

## 🖥️ Giao diện web tiếng Việt

Trang web trên ESP32 hiển thị:

- 📍 **Vĩ độ**
- 🧭 **Kinh độ**
- 🛰️ **Số vệ tinh**
- 📡 **Trạng thái GNSS**
- 🗺️ **Bản đồ OpenStreetMap**
- 🚗 **Vị trí hiện tại**
- 🛣️ **Đường hành trình**
- ⏱️ **Tự động cập nhật định kỳ**

Khi chưa có tọa độ GNSS hợp lệ, bản đồ giữ ở chế độ tổng quan và giao diện báo trạng thái **“Đang chờ tín hiệu GNSS”**.

---

## 📺 Thông tin trên OLED

Để bảo đảm tương thích với font mặc định của SSD1306, chuỗi hiển thị trên OLED dùng chữ ASCII ngắn gọn:

- `VI DO`
- `KINH DO`
- `VT` — số vệ tinh
- trạng thái Wi‑Fi và địa chỉ IP

Phần web vẫn hiển thị tiếng Việt có dấu đầy đủ.

---

## 🚗 Ứng dụng

- 🚘 Theo dõi phương tiện
- 🚚 Giám sát đội xe
- 📦 Theo dõi tài sản
- 🆘 Thiết bị an toàn cá nhân
- 🌐 Giám sát vị trí IoT
- 🥾 Dẫn đường ngoài trời
- 🗺️ Thu thập dữ liệu thực địa GIS

---

## 🧪 Kiểm tra nhanh

Sau khi nạp chương trình:

1. Mở **Serial Monitor** ở baud `115200`.
2. Chờ thiết bị kết nối Wi‑Fi.
3. Ghi lại địa chỉ IP được in ra.
4. Mở IP đó trên trình duyệt của thiết bị cùng mạng.
5. Đưa mô-đun GNSS ra khu vực thoáng để bắt vệ tinh.
6. Kiểm tra marker và hành trình trên bản đồ.

---

## 📖 Nguồn hướng dẫn

Dự án được phát triển dựa trên hướng dẫn kỹ thuật của **PlayWithCircuit**, gồm sơ đồ mạch, giải thích GNSS, kiểm thử và xử lý lỗi.

Phần Việt hóa và cải tiến giao diện trong repository này tập trung vào khả năng sử dụng thuận tiện hơn cho người dùng Việt Nam, đồng thời giữ nguyên nguyên lý hoạt động cốt lõi của hệ thống.

---

## ⭐ Đóng góp

Nếu dự án hữu ích, bạn có thể:

- ⭐ Star repository
- 🍴 Fork để phát triển phiên bản riêng
- 🐛 Mở Issue khi phát hiện lỗi
- 🔧 Gửi Pull Request để cải tiến
- 📢 Chia sẻ với cộng đồng IoT/GIS Việt Nam
