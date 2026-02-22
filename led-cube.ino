#include <WiFi.h>
#include <TFT_eSPI.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "CommunityFibre10Gb_897A3_2";
const char* password = "w87kttmdfz";

TFT_eSPI tft = TFT_eSPI();

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        tft.println("Connecting to WiFi......");
    }
    tft.println("Connected to WiFi");
}

void loop() {
    tft.fillScreen(TFT_BLACK); // Clear screen
    HTTPClient http;
    http.begin("http://192.168.1.19:3000/data");
    int httpCode = http.GET();
    String payload = "";
    if (httpCode > 0) {
        payload = http.getString();
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
            
        if (error) {
          return;
        }

        JsonArray array = doc.as<JsonArray>();

        for (JsonObject obj : array) {
          const char* message = obj["message"]; 
          int x = obj["x"];
          int y = obj["y"];
          int font = obj["font"];
          int size = obj["size"];
          const char* color = obj["color"];
          uint16_t tftColor = convertRGBto565(color); 
          tft.setTextSize(size);
          tft.setTextColor(tftColor);
          tft.drawString(message, x, y, font);
        }
    }
    http.end();

    delay(60000);
}

// Function to convert hex color to RGB565 format
uint16_t convertRGBto565(String hexColor) {
    long number = strtol(&hexColor[1], NULL, 16);  // Convert hex to long
    uint8_t r = (number >> 16) & 0xFF;
    uint8_t g = (number >> 8) & 0xFF;
    uint8_t b = number & 0xFF;
    return tft.color565(r, g, b);  // Convert RGB to 16-bit color
}