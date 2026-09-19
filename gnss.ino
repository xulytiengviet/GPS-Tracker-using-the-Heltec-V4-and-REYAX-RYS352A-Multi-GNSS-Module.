#if 1
#include "HT_TinyGPS++.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"
#include <WiFi.h>
#include <WebServer.h>

// Chân điều khiển nguồn cho mô-đun GNSS trên đầu nối P3 của Heltec V4.
#define VGNSS_CTRL 34
// Chân điều khiển nguồn cho màn hình OLED.
#define OLED_CTRL  36

// UART giao tiếp với mô-đun GNSS.
#define GPS_RX 38
#define GPS_TX 39

HardwareSerial GPS(1);
TinyGPSPlus gps;

static SSD1306Wire display(
    0x3c,
    500000,
    SDA_OLED,
    SCL_OLED,
    GEOMETRY_128_64,
    RST_OLED);

// 🔐 Hãy thay bằng Wi-Fi của bạn trước khi nạp chương trình.
// Không nên lưu mật khẩu Wi-Fi thật trong repository công khai.
char ssid[] = "TEN_WIFI";
char pass[] = "MAT_KHAU_WIFI";

WebServer server(80);

// Dữ liệu GNSS mới nhất.
double latitude  = 0.00;
double longitude = 0.00;
int satellites   = 0;

// API JSON phục vụ giao diện WebGIS.
void handleGPS()
{
    String json =
        "{\"lat\":" +
        String(latitude, 6) +
        ",\"lon\":" +
        String(longitude, 6) +
        ",\"sat\":" +
        String(satellites) +
        "}";

    server.send(200, "application/json; charset=utf-8", json);
}

// Trang WebGIS tiếng Việt chạy trực tiếp trên ESP32.
void handleRoot()
{
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta name="theme-color" content="#0f172a">
<title>🛰️ Bộ theo dõi GNSS Heltec V4</title>

<link rel="stylesheet"
href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css"/>

<script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>

<style>
:root {
    color-scheme: light;
    --bg: #f8fafc;
    --card: #ffffff;
    --text: #0f172a;
    --muted: #64748b;
    --border: #e2e8f0;
    --accent: #2563eb;
    --ok: #15803d;
    --wait: #b45309;
}

* { box-sizing: border-box; }

body {
    margin: 0;
    font-family: Arial, Helvetica, sans-serif;
    background: var(--bg);
    color: var(--text);
}

.app {
    max-width: 1100px;
    margin: 0 auto;
    padding: 16px;
}

.header {
    display: flex;
    gap: 12px;
    align-items: center;
    justify-content: space-between;
    flex-wrap: wrap;
    margin-bottom: 14px;
}

.title {
    margin: 0;
    font-size: clamp(22px, 4vw, 32px);
}

.subtitle {
    margin: 4px 0 0;
    color: var(--muted);
    font-size: 14px;
}

.status {
    display: inline-flex;
    align-items: center;
    gap: 8px;
    padding: 8px 12px;
    border-radius: 999px;
    background: #fff7ed;
    color: var(--wait);
    border: 1px solid #fed7aa;
    font-weight: 700;
    font-size: 14px;
}

.status.ok {
    background: #f0fdf4;
    color: var(--ok);
    border-color: #bbf7d0;
}

.grid {
    display: grid;
    grid-template-columns: repeat(3, minmax(0, 1fr));
    gap: 10px;
    margin-bottom: 12px;
}

.card {
    background: var(--card);
    border: 1px solid var(--border);
    border-radius: 14px;
    padding: 14px;
    box-shadow: 0 4px 16px rgba(15, 23, 42, 0.05);
}

.card-label {
    color: var(--muted);
    font-size: 13px;
    margin-bottom: 8px;
}

.card-value {
    font-size: clamp(19px, 4vw, 26px);
    font-weight: 800;
    word-break: break-word;
}

#map {
    height: min(64vh, 620px);
    min-height: 430px;
    width: 100%;
    border-radius: 16px;
    border: 1px solid var(--border);
    overflow: hidden;
    box-shadow: 0 8px 24px rgba(15, 23, 42, 0.08);
}

.footer {
    display: flex;
    justify-content: space-between;
    gap: 12px;
    flex-wrap: wrap;
    color: var(--muted);
    font-size: 12px;
    padding: 10px 2px 0;
}

.gnss-marker {
    width: 38px;
    height: 38px;
    display: grid;
    place-items: center;
    border-radius: 50%;
    background: #ffffff;
    border: 2px solid var(--accent);
    box-shadow: 0 4px 14px rgba(15, 23, 42, 0.24);
    font-size: 21px;
}

@media (max-width: 720px) {
    .grid { grid-template-columns: 1fr; }
    #map { min-height: 380px; height: 56vh; }
}
</style>
</head>

<body>
<div class="app">
    <div class="header">
        <div>
            <h1 class="title">🛰️ Bộ theo dõi GNSS Heltec V4</h1>
            <p class="subtitle">📡 REYAX RYS352A · 🗺️ Leaflet + OpenStreetMap · ⏱️ cập nhật mỗi 3 giây</p>
        </div>
        <div id="status" class="status">🟠 Đang chờ tín hiệu GNSS</div>
    </div>

    <div class="grid">
        <div class="card">
            <div class="card-label">📍 Vĩ độ</div>
            <div id="lat" class="card-value">--</div>
        </div>

        <div class="card">
            <div class="card-label">🧭 Kinh độ</div>
            <div id="lon" class="card-value">--</div>
        </div>

        <div class="card">
            <div class="card-label">🛰️ Vệ tinh đang dùng</div>
            <div id="sat" class="card-value">0</div>
        </div>
    </div>

    <div id="map"></div>

    <div class="footer">
        <span>🚗 Đường màu xanh: hành trình gần nhất</span>
        <span id="updated">⏱️ Chưa nhận dữ liệu</span>
    </div>
</div>

<script>
const map = L.map('map').setView([16.047079, 108.206230], 5);

L.tileLayer(
    'https://tile.openstreetmap.org/{z}/{x}/{y}.png',
    {
        maxZoom: 19,
        attribution: '&copy; OpenStreetMap contributors'
    }
).addTo(map);

const pathPoints = [];
const trail = L.polyline(pathPoints, {
    color: '#2563eb',
    weight: 4,
    opacity: 0.9
}).addTo(map);

const icon = L.divIcon({
    className: '',
    html: '<div class="gnss-marker">📍</div>',
    iconSize: [38, 38],
    iconAnchor: [19, 38]
});

let marker = null;
let firstFix = true;

function isValidFix(data) {
    return Number.isFinite(Number(data.lat)) &&
           Number.isFinite(Number(data.lon)) &&
           !(Number(data.lat) === 0 && Number(data.lon) === 0);
}

function setWaiting(message) {
    const status = document.getElementById('status');
    status.className = 'status';
    status.textContent = '🟠 ' + message;
}

function setReady() {
    const status = document.getElementById('status');
    status.className = 'status ok';
    status.textContent = '🟢 GNSS đang hoạt động';
}

async function updateGPS() {
    try {
        const response = await fetch('/gps', { cache: 'no-store' });
        if (!response.ok) throw new Error('HTTP ' + response.status);

        const data = await response.json();

        document.getElementById('lat').textContent =
            Number(data.lat).toFixed(6);
        document.getElementById('lon').textContent =
            Number(data.lon).toFixed(6);
        document.getElementById('sat').textContent = data.sat;
        document.getElementById('updated').textContent =
            '⏱️ Cập nhật: ' + new Date().toLocaleTimeString('vi-VN');

        if (!isValidFix(data)) {
            setWaiting('Đang chờ tín hiệu GNSS');
            return;
        }

        setReady();

        const point = [Number(data.lat), Number(data.lon)];

        if (!marker) {
            marker = L.marker(point, { icon }).addTo(map);
            marker.bindPopup('📍 Vị trí GNSS hiện tại');
        } else {
            marker.setLatLng(point);
        }

        pathPoints.push(point);

        if (pathPoints.length > 1000) {
            pathPoints.shift();
        }

        trail.setLatLngs(pathPoints);

        if (firstFix) {
            map.setView(point, 17);
            firstFix = false;
        } else {
            map.panTo(point, { animate: true, duration: 0.5 });
        }
    } catch (error) {
        setWaiting('Mất kết nối với thiết bị');
        document.getElementById('updated').textContent =
            '⚠️ Không lấy được dữ liệu';
        console.error(error);
    }
}

setInterval(updateGPS, 3000);
updateGPS();
</script>
</body>
</html>
)rawliteral";

    server.send(200, "text/html; charset=utf-8", html);
}

void setup()
{
    int x_cord = 0;
    int y_cord = 30;

    Serial.begin(115200);

    // Bật nguồn GNSS.
    pinMode(VGNSS_CTRL, OUTPUT);
    digitalWrite(VGNSS_CTRL, LOW);

    delay(500);

    // Bật OLED.
    pinMode(OLED_CTRL, OUTPUT);
    digitalWrite(OLED_CTRL, LOW);

    display.init();
    display.clear();
    display.display();

    // Khởi động UART của GNSS.
    GPS.begin(115200, SERIAL_8N1, GPS_RX, GPS_TX);

    Serial.println("GNSS da khoi dong");
    display.drawString(0, 0, "GNSS da khoi dong");
    display.display();

    // Kết nối Wi-Fi bằng SSID và mật khẩu cấu hình ở đầu tệp.
    WiFi.begin(ssid, pass);

    Serial.print("Dang ket noi WiFi");
    display.drawString(0, 20, "Dang ket noi WiFi");
    display.display();

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        display.drawString(x_cord, y_cord, ".");
        display.display();
        x_cord += 2;

        if (x_cord > 128)
        {
            x_cord = 0;
            y_cord += 10;
        }
    }

    display.clear();
    display.display();

    Serial.println();
    Serial.println("WiFi da ket noi");
    Serial.print("Dia chi IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/gps", handleGPS);
    server.begin();

    Serial.println("Web server da san sang");

    display.drawString(0, 0,  "WiFi da ket noi");
    display.drawString(0, 20, "Dia chi IP:");
    display.drawString(0, 30, WiFi.localIP().toString());
    display.drawString(0, 50, "Web server san sang");
    display.display();
}

void loop()
{
    // Xử lý yêu cầu HTTP từ trình duyệt.
    server.handleClient();

    unsigned char gpsdata;

    while (GPS.available())
    {
        gpsdata = GPS.read();
        gps.encode(gpsdata);
        Serial.write(gpsdata);
    }

    if (gps.location.isUpdated())
    {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        satellites = gps.satellites.value();

        Serial.print("Vi do: ");
        Serial.print(latitude, 6);
        Serial.print("  Kinh do: ");
        Serial.print(longitude, 6);
        Serial.print("  Ve tinh: ");
        Serial.println(satellites);

        display.clear();
        display.setFont(ArialMT_Plain_10);

        // OLED dùng chuỗi ASCII ngắn để tương thích font mặc định.
        display.drawString(0, 0, "DU LIEU GPS");

        display.drawString(0, 20, "VI DO:");
        display.drawString(38, 20, String(latitude, 6));

        display.drawString(0, 32, "KINH DO:");
        display.drawString(38, 32, String(longitude, 6));

        display.drawString(0, 46, "VT:");
        display.drawString(38, 46, String(satellites));

        display.display();
    }
}
#endif
