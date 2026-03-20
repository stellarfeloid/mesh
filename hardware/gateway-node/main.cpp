#ifndef MESH_CONFIG_H
#define MESH_CONFIG_H

#include <Arduino.h>

// Custom SPI and LoRa Pins for ESP32-C3 SuperMini
#define SCK_PIN   4
#define MISO_PIN  5
#define MOSI_PIN  6
#define SS_PIN    7
#define RST_PIN   10
#define DIO0_PIN  2

// LoRa Settings
#define LORA_FREQ 868E6

// The Binary Struct Payload (13 Bytes Total)
// #pragma pack prevents the compiler from adding padding bytes
#pragma pack(push, 1)
struct MeshPacket {
    uint32_t messageId;    // 4 bytes (for deduplication)
    uint16_t senderId;     // 2 bytes (Node ID)
    uint16_t batteryMv;    // 2 bytes (Battery voltage in millivolts)
    float temperature;     // 4 bytes (Sensor data)
    uint8_t hopCount;      // 1 byte  (Mesh routing limit)
};
#pragma pack(pop)

#endif // MESH_CONFIG_H
