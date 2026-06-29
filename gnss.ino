#if 1
#include "HT_TinyGPS++.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"
#include <WiFi.h>
#include <WebServer.h>

// this pins controls the power to the GNSS module, the GNSS mosule is connected to connector P3 of the heltec V4 board
#define VGNSS_CTRL 34
// this pin controls the power to the OLED module
#define OLED_CTRL  36

// pins for receiving and transmistting data from/to the GNSS module
#define GPS_RX 38
#define GPS_TX 39

// create a UART communication object to communicate with GNSS module over UART 1
HardwareSerial GPS(1);

// create a gps object to pase NMEA data from GNSS module
TinyGPSPlus gps;

// crete a display object to drive on board 128x64 OLED display
static SSD1306Wire display( 0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

// wifi ssid and password
char ssid[] = "Narzo";
char pass[] = "Hotspot@123";

// create an HTTP web server object named server that listens for browser requests on port 80 and allows the ESP32 to host web pages
WebServer server(80);

// variables to save GPS data
double latitude  = 0.00;
double longitude = 0.00;
int satellites   = 0;

// this function is the GPS data provider for our webpage, when browser vists our page then this function runs and sends back the latest GPS information.
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

    server.send(200, "application/json", json);
}

// this function  Creates an HTML webpage and Sends that webpage to the browser
void handleRoot()
{
    String html = R"rawliteral(

    <!DOCTYPE html>
    <html>

    <head>

    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <title>Heltec GNSS Tracker</title>

    <link rel="stylesheet"
    href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css"/>

    <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>

    <style>

    body
    {
        font-family: Arial;
        margin: 10px;
    }

    #map
    {
        height: 500px;
        width: 100%;
    }

    .info
    {
        font-size: 20px;
        margin: 10px 0;
    }

    </style>

    </head>

    <body>

    <h2>Heltec GNSS Tracker</h2>

    <div class="info">
    Latitude :
    <span id="lat">0</span>
    </div>

    <div class="info">
    Longitude :
    <span id="lon">0</span>
    </div>

    <div class="info">
    Satellites :
    <span id="sat">0</span>
    </div>

    <div id="map"></div>

    <script>

    var map = L.map('map').setView([0.00,0.00], 18);

    L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', { maxZoom:19 }).addTo(map);

    var marker = L.marker( [18.5204,73.8567] ).addTo(map);

    var pathPoints = [];

    var trail = L.polyline(pathPoints,{color:'red', weight:4 }).addTo(map);

    function updateGPS()
    {
        fetch('/gps')
        .then(response => response.json())
        .then(data =>
        {
            document.getElementById('lat').innerHTML = data.lat;
            document.getElementById('lon').innerHTML = data.lon;
            document.getElementById('sat').innerHTML = data.sat;

            var point = [data.lat,data.lon];
            marker.setLatLng(point);
            pathPoints.push(point);
            if(pathPoints.length > 1000)
            {
                pathPoints.shift();
            }

            trail.setLatLngs(pathPoints);
            map.panTo(point);
        });
    }

    setInterval(updateGPS, 3000);

    updateGPS();

    </script>

    </body>
    </html>

    )rawliteral";

    server.send(200, "text/html", html);
}

void setup()
{
    int x_cord = 0;
    int y_cord = 30;
    // init uart at baudrate 115200 bps
    Serial.begin(115200);

    // Enable GNSS power
    pinMode(VGNSS_CTRL, OUTPUT);
    digitalWrite(VGNSS_CTRL, LOW);

    delay(500);

    // OLED ON
    pinMode(OLED_CTRL, OUTPUT);
    digitalWrite(OLED_CTRL, LOW);

    display.init();
    display.clear();
    display.display();

    // Start GPS UART
    GPS.begin(115200, SERIAL_8N1, GPS_RX, GPS_TX);

    Serial.println("GNSS Started");

    display.drawString(0, 0, "GNSS Started");
    display.display();

    // connect to wifi network using ssid and password provided
    WiFi.begin(ssid, pass);

    Serial.print("Connecting");
    display.drawString(0, 20, "Connecting to wifi");
    display.display();

    // print ..... on serial if not connected to wifi
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        display.drawString(x_cord, y_cord, ".");
        display.display();
        x_cord+=2;
        if(x_cord>128)
        {
            x_cord = 0;
            y_cord = y_cord + 10;
        }
    }

    display.clear();
    display.display();

    Serial.println();
    Serial.println("WiFi Connected");


    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/gps", handleGPS);

    server.begin();

    Serial.println("Web Server Started");

    display.drawString(0, 0,  "WiFi Connected");
    display.drawString(0, 20, "IP Address : ");
    display.drawString(0, 30, WiFi.localIP().toString());
    display.drawString(0, 50, "Web Server Started");
    display.display();
}

void loop()
{
    // this function continuously checks if there is a request from browser
    server.handleClient();

    unsigned char gpsdata;
    while (GPS.available())
    {
        gpsdata = GPS.read();
        // this finction gets the gps data and parse it character by character   
        gps.encode(gpsdata);
        Serial.write(gpsdata);
    }

    if (gps.location.isUpdated())
    {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        satellites = gps.satellites.value();

        Serial.print("Latitude : ");
        Serial.print(latitude, 6);

        Serial.print("  Longitude : ");
        Serial.print(longitude, 6);

        Serial.print("  Satellites : ");
        Serial.println(satellites);

        display.clear();

        display.setFont(ArialMT_Plain_10);

        display.drawString(0, 0, "GPS Data");

        display.drawString(0, 20, "LAT:");

        display.drawString(30, 20, String(latitude, 6));

        display.drawString(0, 30, "LON:");

        display.drawString(30, 30, String(longitude, 6));

        display.drawString(0, 40, "SAT:");

        display.drawString(30, 40, String(satellites));

        display.display();
    }
}
#elif 0
#include "HT_TinyGPS++.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#define VGNSS_CTRL 34

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

void setup()
{
    Serial.begin(115200);

    // Enable GNSS power
    pinMode(VGNSS_CTRL, OUTPUT);
    digitalWrite(VGNSS_CTRL, LOW);

    delay(500);

    // OLED ON
    pinMode(36, OUTPUT);
    digitalWrite(36, LOW);

    display.init();
    display.clear();
    display.display();

    // Start GPS UART
    GPS.begin(
        115200,
        SERIAL_8N1,
        GPS_RX,
        GPS_TX);

    Serial.println("GNSS Started");

    display.drawString(0, 0, "GNSS Started");
    display.display();
}

void loop()
{
    unsigned char gpsdata;
    while (GPS.available())
    {
        gpsdata = GPS.read();   
        gps.encode(gpsdata);
        Serial.write(gpsdata);
    }

    if (gps.location.isUpdated())
    {
        double lat = gps.location.lat();
        double lon = gps.location.lng();

        Serial.print("Latitude : ");
        Serial.println(lat, 6);

        Serial.print("Longitude: ");
        Serial.println(lon, 6);

        display.clear();

        display.setFont(ArialMT_Plain_10);

        display.drawString(0, 0, "GPS FIX");

        display.drawString(
            0,
            20,
            "LAT:");

        display.drawString(
            30,
            20,
            String(lat, 6));

        display.drawString(
            0,
            40,
            "LON:");

        display.drawString(
            30,
            40,
            String(lon, 6));

        display.display();
    }
}
#else

#include "HT_TinyGPS++.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#define VGNSS_CTRL 34

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

#include <WiFi.h>
#include <WebServer.h>

char ssid[] = "Narzo";
char pass[] = "Hotspot@123";

WebServer server(80);

// Simulated GPS data
double latitude  = 18.520400;
double longitude = 73.856700;
int satellites   = 10;

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

    server.send(200, "application/json", json);
}

void handleRoot()
{
    String html = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<title>Heltec GNSS Tracker</title>

<link rel="stylesheet"
href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css"/>

<script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>

<style>

body
{
    font-family: Arial;
    margin: 10px;
}

#map
{
    height: 500px;
    width: 100%;
}

.info
{
    font-size: 20px;
    margin: 10px 0;
}

</style>

</head>

<body>

<h2>Heltec GNSS Tracker</h2>

<div class="info">
Latitude :
<span id="lat">0</span>
</div>

<div class="info">
Longitude :
<span id="lon">0</span>
</div>

<div class="info">
Satellites :
<span id="sat">0</span>
</div>

<div id="map"></div>

<script>

var map =
L.map('map').setView(
[18.5204,73.8567],
18);

L.tileLayer(
'https://tile.openstreetmap.org/{z}/{x}/{y}.png',
{
maxZoom:19
}).addTo(map);

var marker =
L.marker(
[18.5204,73.8567]
).addTo(map);

var pathPoints = [];

var trail =
L.polyline(
pathPoints,
{
color:'red',
weight:4
}).addTo(map);

function updateGPS()
{
    fetch('/gps')
    .then(response => response.json())
    .then(data =>
    {
        document.getElementById('lat').innerHTML =
            data.lat;

        document.getElementById('lon').innerHTML =
            data.lon;

        document.getElementById('sat').innerHTML =
            data.sat;

        var point =
            [data.lat,data.lon];

        marker.setLatLng(point);

        pathPoints.push(point);

        if(pathPoints.length > 100)
        {
            pathPoints.shift();
        }

        trail.setLatLngs(pathPoints);

        map.panTo(point);
    });
}

setInterval(
updateGPS,
1000);

updateGPS();

</script>

</body>
</html>

)rawliteral";

    server.send(200, "text/html", html);
}

void setup()
{

    Serial.begin(115200);

    // Enable GNSS power
    pinMode(VGNSS_CTRL, OUTPUT);
    digitalWrite(VGNSS_CTRL, LOW);

    delay(500);

    // OLED ON
    pinMode(36, OUTPUT);
    digitalWrite(36, LOW);

    display.init();
    display.clear();
    display.display();

    // Start GPS UART
    GPS.begin(
        115200,
        SERIAL_8N1,
        GPS_RX,
        GPS_TX);

    Serial.println("GNSS Started");

    display.drawString(0, 0, "GNSS Started");
    display.display();

    WiFi.begin(ssid, pass);

    Serial.print("Connecting");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected");

    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/gps", handleGPS);

    server.begin();

    Serial.println("Web Server Started");
}

void loop()
{
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

        Serial.print("Latitude : ");
        Serial.print(latitude, 6);

        Serial.print("  Longitude : ");
        Serial.print(longitude, 6);

        Serial.print("  Satellites : ");
        Serial.println(satellites);

        display.clear();

        display.setFont(ArialMT_Plain_10);

        display.drawString(0, 0, "GPS FIX");

        display.drawString(
            0,
            20,
            "LAT:");

        display.drawString(
            30,
            20,
            String(latitude, 6));

        display.drawString(
            0,
            40,
            "LON:");

        display.drawString(
            30,
            40,
            String(longitude, 6));

        display.display();
    }
}

#endif