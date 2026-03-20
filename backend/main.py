from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import uvicorn
import datetime

# Initialize the FastAPI app
app = FastAPI(title="CommuniMesh SA Backend")

# Define the expected JSON structure using Pydantic
class MeshPayload(BaseModel):
    message_id: int
    sender_id: int
    battery_mv: int
    temperature: float
    hop_count: int
    rssi: int

# Create the POST endpoint
@app.post("/api/v1/telemetry")
async def receive_telemetry(payload: MeshPayload):
    # In the future, you will connect your SQLite/PostgreSQL database here.
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    print("\n" + "="*35)
    print(f"📡 [{timestamp}] Data from Node {payload.sender_id}")
    print("="*35)
    print(f"Message ID:  {payload.message_id}")
    print(f"Temperature: {payload.temperature:.2f} °C")
    print(f"Battery:     {payload.battery_mv} mV")
    print(f"Hops:        {payload.hop_count}")
    print(f"Signal:      {payload.rssi} dBm (RSSI)")
    print("="*35 + "\n")
    
    # Respond to the ESP32 Gateway to confirm receipt
    return {"status": "success", "message": "Telemetry logged"}

# Run the server
if __name__ == "__main__":
    # Binding to 0.0.0.0 allows external network access
    uvicorn.run(app, host="0.0.0.0", port=8000)
