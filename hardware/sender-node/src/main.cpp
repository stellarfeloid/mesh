#include <SPI.h>
#include <LoRa.h>
#include "mesh_config.h"

// Create an instance of our 13-byte payload struct
MeshPacket myPayload;

void setup() {
  Serial.begin(115200);
  
  // Wait up to 3 seconds for USB CDC Serial connection (Crucial for C3 SuperMini)
  uint32_t t = millis();
  while (!Serial && (millis() - t < 3000)); 
  
  Serial.println("\nCommuniMesh SA: Sender Node Starting...");

  // 1. Initialize custom SPI pins
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  
  // 2. Pass the custom pins to the LoRa library
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  // 3. Initialize the radio at 868MHz
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Starting LoRa failed! Check wiring and power.");
    while (1);
  }

  // Configure static information for this specific node
  myPayload.senderId = 101; // Example: Assign this node ID 101
  myPayload.messageId = 0;
  myPayload.hopCount = 0;   // Starts at 0, increases if relayed by other nodes

  Serial.println("LoRa Initialized Successfully. Beginning transmission cycle...");
}

void loop() {
  // 1. Update the dynamic data in the struct
  myPayload.messageId++;
  
  // Mock data for testing (later replace with actual sensor readings)
  myPayload.batteryMv = random(3200, 4200); // Mock LiPo voltage between 3.2V and 4.2V
  myPayload.temperature = 22.5 + (random(-15, 15) / 10.0); // Mock temp fluctuating around 22.5C

  Serial.printf("Sending Packet ID %u from Node %u...\n", myPayload.messageId, myPayload.senderId);

  // 2. Blast the raw 13 bytes over LoRa
  LoRa.beginPacket();
  
  // Cast the struct's memory address to a byte pointer, and send its exact size
  LoRa.write((uint8_t *)&myPayload, sizeof(myPayload));
  
  LoRa.endPacket();

  Serial.println("Packet sent. Sleeping for 10 seconds.");
  
  // Wait 10 seconds before sending the next packet
  delay(10000); 
}
