#include <Arduino.h>              // Arduino Bibliothek                 
#include <Adafruit_ST7735.h>      // Adafruit ST7735 Bibliothek
#include <TinyGPSPlus.h>          // TinyGPS++ Bibliothek
#include <WiFi.h>                 // WiFi Bibliothek
#include <sMQTTBroker.h>          // sMQTTBroker Bibliothek
#include <ArduinoJson.h>          // ArduinoJson Bibliothek
#include <Wire.h>                 // Wire Bibliothek
#define LED_BUILTIN 18            // LED_BUILTIN Pin
#define VBAT_PIN 1                // Heltec GPIO  VBat Spannungsmessung
#define VBAT_READ_CNTRL_PIN 37    // Heltec GPIO  VBat Spannungsmessung Control (Also, take care NOT to have ADC read connection in OPEN DRAIN when GPIO goes HIGH)
#define ADC_READ_STABILIZE 10     // in ms (delay from GPIO control and ADC connections times)
// GNSS
#define VEXT_CTL 3                // GNSS GPIO  VEXT_CTL
#define GPS_RX 33                 // GNSS GPIO  GPS_RX
#define GPS_TX 34                 // GNSS GPIO  GPS_TX
// Display
#define TFT_LED  21               // Display GPIO  TFT_LED
#define TFT_CS   38               // Display GPIO  TFT_CS
#define TFT_RST  39               // Display GPIO  TFT_RST
#define TFT_DC   40               // Display GPIO  TFT_DC
#define TFT_SCLK 41               // Display GPIO  TFT_SCLK
#define TFT_MOSI 42               // Display GPIO  TFT_MOSI
// Wifi
#define WIFI_SSID     "ESP32_AP_" // WiFi SSID
#define WIFI_PASSWORD "12345678"  // WiFi Passwort
// MQTT
#define MQTT_PORT      1883       // MQTT Port
sMQTTBroker Broker;               // sMQTTBroker Instanz
uint64_t chipid = ESP.getEfuseMac();         // MAC address (laenge: 6 bytes).
String chipID_str = String(chipid, HEX);     // MAC address als String
String short_chipID_str = chipID_str.substring(0,4);  // MAC address als String (nur die ersten 5 Zeichen)

TinyGPSPlus gps;                  // TinyGPSPlus Instanz
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST); // Adafruit_ST7735 Instanz

void setup_gps() {          // GPS Initialisierung
    Serial1.begin(115200, SERIAL_8N1, GPS_RX, GPS_TX);
}

void setup_wifi() {        // WiFi Initialisierung
    Serial.print(F("[ESP32] Initialisiere WiFi ... "));
    WiFi.softAP((WIFI_SSID + short_chipID_str).c_str(), WIFI_PASSWORD);
    Serial.print(F("Erfolgreich!\n"));

}
void setup_mqtt() {       // MQTT Initialisierung
    Serial.print(F("[ESP32] Initialisiere sMQTT ... "));
    Broker.init(MQTT_PORT);
    Serial.print(F("Erfolgreich!\n"));
}
void setup_json() {       
        //Json init
}
float readBatLevel() {   // Batterie Spannungsmessung
  int analogValue = analogRead(VBAT_PIN);
  float voltage = 0.0041 * analogValue;
  return voltage;
}

void setup_tft() {          // Display Initialisierung
    pinMode(TFT_CS, OUTPUT);            
    pinMode(TFT_RST, OUTPUT);   
    tft.initR(INITR_MINI160x80_PLUGIN);         
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    delay(50);
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);
}
void setup() {         // Setup
    Serial.begin(115200, SERIAL_8N1);
    Serial.println("[DEBUG] Serial Monitor gestartet...");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    pinMode(VEXT_CTL, OUTPUT);
    digitalWrite(VEXT_CTL, HIGH);
    setup_tft();
    setup_gps();
    setup_wifi();
    setup_mqtt();
    setup_json();
 // pinMode(VBAT_READ_CNTRL_PIN,OUTPUT);
  //digitalWrite(VBAT_READ_CNTRL_PIN, LOW);
    tft.setCursor(0, 0);
    tft.print("GPS suche...");
    digitalWrite(LED_BUILTIN, LOW);
}