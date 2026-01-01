# Wokwi Simulation Files

This directory contains the Wokwi simulation project for testing the ESP32-CAM custom board with the robot-spider hexapod configuration.

## Files

- **`board.json`** - Custom ESP32-CAM board definition with pin mappings
- **`board.svg`** - Visual representation of the ESP32-CAM board
- **`diagram.json`** - Wokwi circuit diagram with ESP32-CAM, PCA9685, and servos
- **`sketch.ino`** - Test firmware demonstrating I2C and servo control
- **`wokwi.toml`** - Wokwi CLI configuration
- **`README.md`** - This file

## Custom Board Definition

This simulation uses a custom ESP32-CAM board definition included in this directory:

- **`board.json`** - Pin definitions and component positions
- **`board.svg`** - Visual representation of the board

These files define the custom ESP32-CAM board with I2C on GPIO14/15 matching the hexapod hardware configuration.

## Quick Start

### Option 1: Wokwi Online
1. Visit https://wokwi.com
2. Create a new ESP32 project
3. Press **F1** → "Load custom board file..." → select `board.json` from this directory
4. Upload `diagram.json` and `sketch.ino`
5. Click "Start Simulation"

### Option 2: Wokwi VSCode Extension
1. Install the Wokwi extension in VSCode
2. Open this directory (`docs/wokwi/`)
3. The custom board will be loaded automatically
4. Click "Start Simulation" button

### Option 3: Wokwi CLI
```bash
cd docs/wokwi
wokwi-cli
```

**Note:** All required files including the custom board definition are in this directory.

## Circuit Configuration

### Components
- **ESP32-CAM** (custom board from `/board.json`)
- **PCA9685** PWM driver (I2C address 0x40)
- **4 x Servos** on PWM channels 0-3
- **Status LED** on GPIO4

### I2C Connections
- ESP32-CAM GPIO14 (SCL) → PCA9685 SCL (green wire)
- ESP32-CAM GPIO15 (SDA) → PCA9685 SDA (blue wire)
- ESP32-CAM VCC (3.3V) → PCA9685 VCC (red wire)
- ESP32-CAM GND → PCA9685 GND (black wire)

### Servo Connections
- PCA9685 PWM0-3 → Servo signal pins (orange wires)
- PCA9685 V+ → ESP32-CAM 5V (servo power rail)
- All servos share 5V power and common ground

## Test Firmware Features

The `sketch.ino` demonstrates:

1. **I2C Initialization**
   - Configures GPIO14/15 for I2C communication
   - Scans bus to detect PCA9685 at 0x40

2. **PCA9685 Setup**
   - Initializes PWM driver at 50 Hz (standard servo frequency)
   - Configures 12-bit resolution (4096 steps)

3. **Servo Sweep Demo**
   - Sweeps all 4 servos from 1000μs to 2000μs pulse width
   - Updates position every 20ms
   - Represents one hexapod leg movement

4. **Status Indicators**
   - LED blinks in sync with servo sweep direction
   - Serial output shows position and heap status every second

## Expected Serial Output

```
ESP32-CAM Test - Robot Spider Configuration
============================================
I2C initialized on SDA=GPIO15, SCL=GPIO14

Scanning I2C bus...
  I2C device found at address 0x40
  Found 1 I2C device(s)

Initializing PCA9685...
Setup complete!
Starting servo test sequence...

Servo position: 50% (1500 us)
Free heap: 289456 bytes
Servo position: 75% (1750 us)
Free heap: 289456 bytes
```

## Extending for Full Hexapod

To simulate the complete 12-servo hexapod:

1. **Add more servos to diagram.json:**
   ```json
   {
     "type": "wokwi-servo",
     "id": "servo5",
     "attrs": { "label": "Servo 4 (Leg 2 Femur)" }
   }
   ```

2. **Update connections** for PWM channels 4-11

3. **Modify sketch.ino** to control all 12 servos

4. **Implement gait patterns** from robot-spider firmware

## Notes

- All simulation files including custom board definition are in this directory
- The custom board defines ESP32-CAM with I2C on GPIO14/15
- Camera, PSRAM, and SD card features are not simulated
- Servo current draw is not accurately simulated (real hexapod may need external power)
- Board files (`board.json`, `board.svg`) can be reused for other ESP32-CAM Wokwi projects

## References

- **Robot-Spider Project Root:** `/Users/esumerfd/GoogleDrive/edward/Personal/projects/arduino/robot-spider/`
- **ADR-002:** `../adr/002-hexapod-physical-model-analysis.md` (hexapod migration plan)
- **ESP32-CAM Pinout:** https://www.espboards.dev/esp32/esp32cam/
- **Wokwi Custom Boards:** https://docs.wokwi.com/guides/custom-boards
- **Wokwi PCA9685:** https://docs.wokwi.com/parts/wokwi-pca9685
