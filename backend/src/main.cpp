#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "mesh_config.h"

// --- Wi-Fi Credentials ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// --- FastAPI Endpoint ---
// IMPORTANT: Use your Fedora machine's actual local IP (e.g., 192.168.1.X), not localhost/127.0.0.1
const char* serverUrl = "http://192.168.1.100:8000/api/v1/telemetry";

MeshPacket incomingPayload;

void setup() {
  Serial.begin(115200);
  uint32_t t = millis();
  while (!Serial && (millis() - t < 3000)); 
  
  Serial.println("\nCommuniMesh SA: Gateway Node Starting...");

  // 1. Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected! IP: " + WiFi.localIP().toString());

  // 2. Initialize LoRa
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Starting LoRa failed! Check wiring.");
    while (1);
  }

  Serial.println("LoRa Initialized. Gateway ready.");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    if (packetSize == sizeof(MeshPacket)) {
      
      // Read binary data into the struct
      LoRa.readBytes((uint8_t *)&incomingPayload, packetSize);

      Serial.printf("Valid packet received from Node: %u. Forwarding to FastAPI...\n", incomingPayload.senderId);

      // Only attempt to send if Wi-Fi is still connected
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");

        // Create a JSON document (Using ArduinoJson v7 syntax)
        JsonDocument doc;
        doc["message_id"] = incomingPayload.messageId;
        doc["sender_id"] = incomingPayload.senderId;
        doc["battery_mv"] = incomingPayload.batteryMv;
        doc["temperature"] = incomingPayload.temperature;
        doc["hop_count"] = incomingPayload.hopCount;
        doc["rssi"] = LoRa.packetRssi();

        // Serialize the JSON into a string
        String jsonPayload;
        serializeJson(doc, jsonPayload);

        // Send the HTTP POST request
        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0) {
          Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        } else {
          Serial.printf("Error code: %d\n", httpResponseCode);
        }
        
        // Free resources
        http.end(); 
      } else {
        Serial.println("Wi-Fi disconnected. Cannot forward packet.");
      }
    }
  }
}
