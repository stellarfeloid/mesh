# CommuniMesh SA

CommuniMesh SA is a decentralized, edge-to-server communication network. It bridges low-power, long-range RF (LoRa) environments with high-speed IP networks using a highly efficient binary packet structure and a containerized HTTP backend.

## 🏗️ Architecture Overview

This project is divided into three primary layers:

1. **End Nodes (LoRa Mesh):** Distributed ESP32-C3 modules that collect data and transmit a lightweight 13-byte packed C++ binary struct over 868MHz LoRa to minimize airtime and battery consumption.
2. **Gateway Node (LoRa to Wi-Fi Bridge):** An ESP32-C3 that listens to the LoRa network. It validates the binary struct, unpacks it, converts the data into JSON, and forwards it via an HTTP POST request over Wi-Fi.
3. **Backend API (FastAPI):** A Python-based server that receives, validates (using Pydantic), and logs the incoming JSON telemetry data from the Gateway.

## 🗂️ Repository Structure

\`\`\`text
CommuniMesh-SA/
├── backend/                   # Python FastAPI Environment (Podman/Debian)
│   ├── main.py                # The FastAPI server and Pydantic models
│   └── requirements.txt       # Dependencies: fastapi, uvicorn, pydantic
│
└── hardware/                  # C++ PlatformIO Environment (Distrobox/Ubuntu)
    ├── Gateway-Node/          # The LoRa-to-WiFi Bridge
    │   ├── platformio.ini     
    │   ├── include/
    │   │   └── mesh_config.h  # Pin configs and the 13-byte struct
    │   └── src/
    │       └── main.cpp       # Wi-Fi routing and HTTP POST logic
    │
    └── Sender-Node/           # The LoRa End Node
        ├── platformio.ini     
        ├── include/
        │   └── mesh_config.h  # EXACT COPY of the Gateway's mesh_config.h
        └── src/
            └── main.cpp       # LoRa struct generation and transmission
\`\`\`

## 🛠️ Hardware Requirements

* **Microcontrollers:** ESP32-C3 SuperMini modules.
* **LoRa Transceivers:** Ai-Thinker Ra-01H (SX1276) tuned to 868MHz (ICASA compliant for South Africa).
* **Antennas:** 868MHz tuned antennas for proper VSWR and range.

## 🚀 Getting Started

### 1. Hardware Compilation (PlatformIO)
Navigate to either the `hardware/Sender-Node` or `hardware/Gateway-Node` directory.
\`\`\`bash
# Install dependencies and compile
pio run

# Upload to the ESP32-C3 via USB
pio run -t upload

# Open Serial Monitor
pio device monitor -b 115200
\`\`\`

### 2. Backend Server (FastAPI)
Navigate to the `backend` directory.
\`\`\`bash
# Install Python dependencies
pip install -r requirements.txt

# Run the server (Binding to 0.0.0.0 allows external Gateway access)
uvicorn main:app --host 0.0.0.0 --port 8000
\`\`\`
