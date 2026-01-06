# ADR 002: Hexapod Physical Model Analysis and Migration Plan

## Status
Accepted

## Date
2025-12-30

## Context

We are migrating the test harness from the development environment to the physical hexapod model based on the delta3robotics SIXpack design. This ADR documents the analysis of the existing Robotics-SIXpack project and identifies the changes needed to support the physical hardware.

## Robotics-SIXpack Project Analysis

### Project Overview

**Source:** Downloaded package from https://cults3d.com/de/modell-3d/verschiedene/sixpack   

The SIXpack is a free robotics hexapod design by delta3robotics (https://cults3d.com/de/modell-3d/verschiedene/sixpack) featuring:
- 6 legs with 2 servos each (12 total servos)
- ESP32-CAM module (AI Thinker model)
- PCA9685 PWM servo driver (I2C)
- WiFi access point for web-based control
- Camera streaming capability
- Web interface for remote control

### Hardware Configuration

#### ESP32 Module
**Model:** ESP32-CAM (CAMERA_MODEL_AI_THINKER)

**Camera Pins:**
- PWDN: GPIO32
- XCLK: GPIO0
- SIOD (I2C Data): GPIO26
- SIOC (I2C Clock): GPIO27
- Y9-Y2: GPIO35, 34, 39, 36, 21, 19, 18, 5
- VSYNC: GPIO25
- HREF: GPIO23
- PCLK: GPIO22
- LED_BUILTIN: GPIO4

#### I2C Configuration (PCA9685)
**Original SIXpack Pins:**
```cpp
#define i2c_sda 1    // GPIO1 (conflicts with UART TX)
#define i2c_scl 3    // GPIO3 (conflicts with UART RX)
```

**Critical Issue:** GPIO1 and GPIO3 are the ESP32's UART TX/RX pins. Using them for I2C **breaks serial communication**, making debugging difficult. The original firmware acknowledges this in comments (SIXpackAccesPoint.ino:1134-1139):

> "The serial connection can be used to debug things... with the current setup you must comment out everything that is related to I2C and pwm, since the rx and tx pins are defined as the I2C pins and the I2C communication breaks the serial communication"

#### Servo Configuration
- **Count:** 12 servos (channels 0-11 on PCA9685)
- **Pulse Range:**
  - Minimum: 150 (0°)
  - Maximum: 650 (180°)
- **PWM Frequency:** 60 Hz (standard for servos)
  - Note: Firmware sets 1600 Hz then immediately 60 Hz (lines 1129-1130)
- **Servo Mapping:**
  - Servos 0, 2, 4, 6, 8, 10: Shoulder/hip joints (horizontal movement)
  - Servos 1, 3, 5, 7, 9, 11: Knee joints (vertical movement)

#### Power and Programming
- **Programming:** USB-to-Serial adapter (FTDI FT232RL or similar)
  - Conflicts with I2C when using GPIO1/3
- **Power:** External (not specified in code)
  - Battery pack (voltage/capacity not documented)
  - Powers ESP32-CAM and 12 servos through PCA9685
  - Brownout detector disabled in code (line 1131)

### Physical Model
**CAD Files Available:**
- STL files (for 3D printing): Body, Head, Lid, Caps, Leg parts
- STEP files (for CAD editing): Same components

**Components:**
- Main body housing
- 6 leg assemblies (2-part legs)
- Head/camera mount
- Lid and access caps

## Current Robot-Spider Configuration

### Hardware Platform
**Board:** Standard ESP32 development board (not ESP32-CAM)

### I2C Configuration
**Current Pin Assignment:**
```cpp
#define I2C_SDA 15   // GPIO15
#define I2C_SCL 14   // GPIO14
```

**Advantages:**
- No conflict with UART (GPIO1/3 available for serial debugging)
- More stable development experience
- Maintains serial communication for logging

### Servo Configuration
- **Count:** 12 servos (same as SIXpack)
- **Pulse Range:**
  - Minimum: 150 (matches SIXpack)
  - Maximum: 545 (narrower range than SIXpack's 650)
- **PWM Frequency:** 50 Hz (vs SIXpack's 60 Hz)

## Migration Requirements

### 1. Pinout Compatibility Issues

#### Issue #1: I2C Pin Conflict (RESOLVED)
**Problem:** Physical SIXpack model has I2C connections soldered to GPIO1/3

**Resolution:** Hardware can be modified - soldering iron available, pins can be desoldered and rewired

**Impact Analysis:**

**Original SIXpack (GPIO1/3 for I2C):**
- ❌ Conflicts with UART - breaks serial debugging
- ❌ Conflicts with ESP-prog programming interface
- ❌ Must disable I2C to use serial communication

**Current Robot-Spider (GPIO14/15 for I2C):**
- ✅ UART (GPIO1/3) free for serial debugging
- ✅ UART free for ESP-prog programming interface
- ✅ No conflicts with ESP-prog auto-programming circuit
- ✅ Can use serial monitor and program simultaneously

**Decision:** **Keep GPIO14/15 for I2C** (current configuration)
- Desolder I2C connections from GPIO1/3 on physical model
- Rewire to GPIO14/15 to match firmware
- Maintains full compatibility with ESP-prog programming
- Preserves serial debugging capability
- No firmware changes required

#### Issue #2: Servo Pulse Range Difference
**Problem:**
- SIXpack uses 150-650 pulse width
- Robot-spider uses 150-545 pulse width

**Impact:**
- Narrower range may limit servo movement range
- Could affect gait implementation
- May need servo calibration adjustments

**Decision:** Test with current range first, expand if movement is limited

#### Issue #3: PWM Frequency Difference
**Problem:**
- SIXpack: 60 Hz
- Robot-spider: 50 Hz

**Impact:**
- Both frequencies work for standard servos
- 50 Hz is more standard for servos
- 60 Hz may provide slightly smoother control

**Decision:** Start with 50 Hz, adjust to 60 Hz if needed

### 2. ESP-prog Programming Board Integration

**Goal:** Enable in-place programming while robot is mounted on physical model

**Interface:** ESP-prog Program Interface (UART-based auto-programming)
- Reference: https://docs.espressif.com/projects/esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html

**Requirements:**
1. ESP-prog connector placement
   - Must not interfere with servo operation
   - Should be accessible when robot is assembled
   - May need custom mounting bracket

2. Required Program Interface Pins (6-pin connector):
   - **TXD** - Transmit data to ESP32 RX (GPIO3)
   - **RXD** - Receive data from ESP32 TX (GPIO1)
   - **IO0** - Boot mode control (pulled low for programming mode)
   - **EN** - Reset control (DTR/RTS auto-programming)
   - **GND** - Ground
   - **VCC** - 3.3V or 5V power (optional, can power from ESP-prog)

**Auto-Programming Circuit:**
The ESP-prog uses DTR/RTS signals to automatically control EN and IO0 for seamless programming without manual button presses.

**Pin Analysis:**
- **No GPIO conflicts!** Programming interface only uses UART (GPIO1/3)
- GPIO14/15 (current I2C pins) are **completely free** - no conflict
- Can keep current I2C configuration on GPIO14/15
- Or migrate I2C elsewhere for other reasons (flexibility, better layout)

**Connector Selection: 6-pin JST-XH Connector with Dual Purpose**

**Implementation:**
- **On Hexapod:** 6-pin JST-XH socket mounted on body
- **Cable:** JST connector with ribbon cable for flexibility
- **On ESP-prog:** Corresponding 6-pin JST connector
- **Dual Purpose:** Same connector serves both programming and external power input

**Advantages of JST + Ribbon Cable:**
- ✅ Keyed connection - prevents reverse insertion
- ✅ Robust mechanical connection
- ✅ Easy to disconnect/reconnect for programming sessions
- ✅ Ribbon cable provides flexibility for routing
- ✅ Professional appearance
- ✅ Strain relief from JST housing
- ✅ Single connector simplifies chassis design
- ✅ VCC pin serves dual purpose (3.3V programming or external power)

**Pin Order (JST-XH 2.5mm pitch):**
1. GND (Black)
2. VCC (Red) - **Dual purpose: 3.3V from ESP-prog OR external power input**
3. EN (Yellow)
4. IO0 (Green)
5. RXD (Blue)
6. TXD (White)

**Dual Purpose VCC Pin:**
- **Programming Mode:** ESP-prog provides 3.3V on VCC (sufficient for programming)
- **Power Mode:** External power supply connected to VCC drives ESP32-CAM and servos
- VCC connects to ESP32-CAM 5V pin which internally regulates to 3.3V for logic
- Automatic power selection via diode-OR circuit (VCC or battery, whichever is higher)

**Key Advantage:**
Since programming interface uses UART, it's **compatible with keeping TX/RX (GPIO1/3) free** for both serial debugging and programming. I2C can remain on GPIO14/15 without affecting ESP-prog functionality. The dual-purpose VCC pin eliminates the need for a separate micro-USB power connector.

### 3. External Power Supply Integration via JST Connector

**Goal:** Enable external power input through the same JST connector used for programming

**Design Decision:** Use JST VCC pin for dual-purpose power input
- Eliminates need for separate micro-USB connector
- Simplifies chassis design (single connector instead of two)
- VCC pin accepts either:
  - 3.3V from ESP-prog during programming
  - External power supply (5V-7V) for development/testing

**Power Input Options:**
1. **Programming Mode:** ESP-prog provides 3.3V on VCC
   - Sufficient for programming operations
   - Battery can simultaneously power servos via diode-OR circuit
2. **External Power Mode:** Wall adapter or bench supply connected to VCC
   - Useful for development without depleting battery
   - Easier debugging with stable power source
3. **Battery Mode:** Battery powers system independently
   - For autonomous operation
   - Full current capacity for servo loads

**Current Capacity Considerations:**
- 12 servos can draw significant current (potentially 1-2A each under load)
- External power supply should be rated for adequate current (5A+ recommended)
- Battery pack required for full autonomous operation
- External power suitable for tethered development/testing

**Recommendation:**
- External power (via JST VCC) for development/testing
- Battery pack for full operation and autonomous movement
- Automatic selection via diode-OR circuit (no manual switching)

### 4. Physical Model Modifications

**Planned Additions:**
1. **Dual-purpose JST-XH connector (Programming + Power)**
   - Type: 6-pin JST-XH socket (2.5mm pitch)
   - Location: Accessible side of hexapod body (TBD based on physical inspection)
   - Mounting: 3D-printed bracket or integrated into chassis model
   - Cable: Ribbon cable with JST connectors for ESP-prog or power supply connection
   - Pin assignment: GND, VCC (dual-purpose), EN, IO0, RXD, TXD
   - Functions:
     - Programming: Connect ESP-prog for firmware upload and serial debugging
     - Power: Connect external power supply for development/testing

2. **Automatic power selection circuit**
   - Type: Diode-OR configuration using Schottky diodes
   - Operation: Automatically selects between JST VCC (external power/ESP-prog) and battery
   - Components:
     - Schottky diode on JST VCC input (e.g., 1N5819, low voltage drop ~0.2V)
     - Schottky diode on battery input (prevents backfeed to battery when external power connected)
     - Optional: LED indicator showing which power source is active
   - Advantages:
     - ✅ No manual switching required - plug and go
     - ✅ Automatic failover between power sources
     - ✅ Backfeed protection built-in
     - ✅ Simpler user experience
     - ✅ Fewer mechanical components to fail
     - ✅ Works with both ESP-prog (3.3V) and external power supply

   **Circuit Diagram:**
   ```mermaid
   graph LR
       JST[JST VCC Pin<br/>3.3V-7V] -->|"+"| D1[Schottky Diode<br/>1N5819]
       BAT[Battery Input<br/>6V-7.4V] -->|"+"| D2[Schottky Diode<br/>1N5819]
       D1 --> VBUS[Power Bus<br/>Voltage varies]
       D2 --> VBUS
       VBUS --> ESP32[ESP32-CAM<br/>VIN/5V]
       VBUS --> PCA[PCA9685<br/>VCC]

       JST_GND[JST GND] --> GND[Common Ground]
       BAT_GND[Battery GND] --> GND
       GND --> ESP32_GND[ESP32 GND]
       GND --> PCA_GND[PCA9685 GND]

       VBUS -.->|Optional| LED1[Green LED<br/>External Power]
       VBUS -.->|Optional| LED2[Red LED<br/>Battery Active]

       style JST fill:#4CAF50,color:#fff
       style BAT fill:#D32F2F,color:#fff
       style VBUS fill:#F57C00,color:#fff
       style D1 fill:#1976D2,color:#fff
       style D2 fill:#1976D2,color:#fff
       style LED1 fill:#2E7D32,color:#fff,stroke-dasharray: 5 5
       style LED2 fill:#C62828,color:#fff,stroke-dasharray: 5 5
   ```

   - Note: Higher voltage source takes priority automatically
   - ESP-prog (3.3V): Battery powers servos, ESP-prog provides logic power
   - External power (5-7V): External power dominates if voltage higher than battery
   - Battery only: Battery powers entire system
   - Voltage drop: ~0.2V across each Schottky diode

## Implementation Plan

### Phase 1: Physical Inspection
1. Obtain physical SIXpack model
2. Document actual wiring of I2C to PCA9685
3. Identify mounting points for ESP-prog connector
4. Assess power input accessibility
5. Photograph and document current state

### Phase 2: I2C Pin Migration (Hardware)
**Decision:** Keep GPIO14/15 for I2C (current firmware configuration)

Hardware modifications required:
1. Desolder I2C connections from GPIO1/3 on physical SIXpack model
2. Rewire SDA to GPIO15 on ESP32
3. Rewire SCL to GPIO14 on ESP32
4. Verify connections with multimeter
5. Test I2C communication with PCA9685
6. Document wiring changes with photos

**No firmware changes required** - board.cpp already configured for GPIO14/15

### Phase 3: Dual-Purpose JST Connector Integration
**Selected Connector:** 6-pin JST-XH connector for both programming and power

Hardware installation steps:
1. Source components:
   - 6-pin JST-XH socket (for hexapod body mount)
   - 6-pin JST connector with pre-crimped ribbon cable
   - Matching JST plug for ESP-prog end
   - Optional: Additional JST plug for external power supply adapter
2. Design and print JST connector mount bracket for hexapod body
   - Should be accessible when robot is assembled
   - Position to avoid servo interference
   - Integrate into chassis 3D model if reprinting
3. Install 6-pin JST socket on hexapod
   - Mount socket to body (epoxy, screws, or 3D-printed bracket)
   - Wire socket pins to ESP32-CAM and power system:
     - Pin 1 (GND) → ESP32-CAM GND and power system GND
     - Pin 2 (VCC) → ESP32-CAM 5V pin (via diode-OR circuit for power mode)
     - Pin 3 (EN) → ESP32-CAM EN pin
     - Pin 4 (IO0) → ESP32-CAM IO0 pin (GPIO0)
     - Pin 5 (RXD) → ESP32-CAM TX (GPIO1)
     - Pin 6 (TXD) → ESP32-CAM RX (GPIO3)
4. Prepare ESP-prog cable
   - Attach JST plug to ESP-prog Program interface
   - Verify pin mapping matches hexapod socket
5. Wire VCC to diode-OR circuit
   - JST VCC (Pin 2) → Schottky diode anode
   - Diode cathode → Power bus
   - Power bus connects to ESP32-CAM VIN and PCA9685 VCC
6. Test connections
   - **Programming test:** Connect ESP-prog, verify auto-programming and serial monitor
   - **Power test:** Connect external power supply via JST VCC, verify system operation
   - **Dual operation:** Test ESP-prog with battery simultaneously connected
7. Document connection procedure with photos and pinout diagram

### Phase 4: Power Selection Circuit Implementation
1. Source components:
   - 2x Schottky diodes (e.g., 1N5819 or similar, rated for 1A+)
   - Optional: Dual-color LED (red=battery, green=external) with current-limiting resistors
   - Heat shrink tubing for wire connections
2. Design and build automatic power selection circuit:
   - Diode-OR configuration:
     - JST VCC (Pin 2) → Schottky diode → Power bus
     - Battery V+ → Schottky diode → Power bus
   - Optional LED indicators for visual feedback
3. Wire diode-OR circuit to power distribution
   - Solder JST VCC wire to diode anode
   - Connect diode cathode to power bus
   - Verify battery already has diode in circuit (or add if needed)
   - Connect power bus output to ESP32-CAM VIN and PCA9685 VCC
   - Ensure proper polarity and common ground connections
   - Add heat shrink tubing to all solder joints
4. Test power selection:
   - Battery only: Verify full operation
   - ESP-prog only (3.3V): Verify programming works, battery powers servos
   - External power (5-7V): Verify operation with servo movement
   - ESP-prog + Battery: Verify both work simultaneously
   - External power + Battery: Verify higher voltage source takes priority
   - Measure voltage drop across diodes (~0.2V for Schottky)
5. Optional: Add power indicator LEDs
   - Green LED for external power active
   - Red LED for battery active
6. Document power system operation and usage guidelines

### Phase 5: Testing and Validation
1. Power-on test (external power via JST and battery)
2. I2C communication verification
3. Individual servo testing
4. Gait execution tests
5. ESP-prog programming and debugging test
6. Dual-purpose connector validation (programming + power modes)
7. Long-duration stability test
8. Document final configuration

## Final Pin Configuration

**Selected Configuration:** GPIO14/15 for I2C with ESP-prog Programming Interface

| Function | GPIO Pins | Notes |
|----------|-----------|-------|
| **I2C (PCA9685)** | GPIO15 (SDA), GPIO14 (SCL) | Current firmware config - no changes needed |
| **UART (Serial/ESP-prog)** | GPIO1 (TX), GPIO3 (RX) | Available for both serial debug and programming |
| **ESP-prog Programming** | TX, RX, IO0, EN, GND, 3.3V | 6-pin connector - no GPIO conflicts |

**Advantages of This Configuration:**
- ✅ No firmware changes required (already using GPIO14/15)
- ✅ Full serial debugging capability (UART free)
- ✅ ESP-prog programming works seamlessly (uses UART + control pins)
- ✅ No pin conflicts between I2C, UART, and ESP-prog
- ✅ Only hardware change: desolder GPIO1/3, rewire to GPIO14/15

**Why Not JTAG?**
- JTAG not required - ESP-prog programming interface (UART-based) is sufficient
- JTAG would conflict with GPIO14/15 (I2C) unnecessarily
- Programming interface provides upload capability and serial debugging
- Simpler hardware integration (6-pin vs 6+ pins for JTAG)

## Risks and Mitigations

### Risk 1: Hardwired I2C Pins (RESOLVED)
**Status:** ✅ Resolved - can desolder and rewire
**Original Impact:** Forced to use GPIO1/3, losing serial debugging
**Resolution:**
- Desolder existing GPIO1/3 connections
- Rewire to GPIO14/15 to match firmware
- No firmware changes needed

### Risk 2: Insufficient External Power Current
**Impact:** Servos brown out or ESP32 resets under load with inadequate external power supply
**Likelihood:** Medium - 12 servos can draw significant current (potentially >>5A under load)
**Mitigation:**
- Use external power supply rated for adequate current (5A+ recommended)
- Warning LED to indicate which power source is active
- Clear documentation stating power supply requirements
- Test with progressively increasing servo load
- Battery pack required for actual robot movement and autonomous operation
- ESP-prog 3.3V output only for programming (battery powers servos simultaneously)

### Risk 3: Desoldering Damage
**Impact:** Damaged PCB traces or pads during I2C pin migration
**Likelihood:** Low-Medium (with proper technique)
**Mitigation:**
- Use quality desoldering equipment (wick, pump, or hot air)
- Apply appropriate temperature (<350°C)
- Test continuity before/after with multimeter
- Have backup ESP32 module available
- Consider using flying wires if pads are damaged

### Risk 4: Physical Space Constraints
**Impact:** Cannot mount JST connector in body
**Likelihood:** Low (single connector is smaller than two separate connectors)
**Mitigation:**
- Design compact mounting solution
- Use right-angle connectors to save space if needed
- Single connector simplifies mounting (vs. previous two-connector design)
- May need to modify body STL and reprint with connector cutout
- 3D-printed bracket option if body modification is difficult

## References

- Robotics-SIXpack Project: `/Users/esumerfd/GoogleDrive/edward/Personal/projects/arduino/Robotics-SIXpack`
- Original SIXpack Design: https://cults3d.com/de/modell-3d/verschiedene/sixpack
- delta3robotics: https://wonderl.ink/@delta3robotics
- ESP32 Pinout Reference: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
- PCA9685 Datasheet: https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf
- ESP-prog Documentation: https://docs.espressif.com/projects/espressif-esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html

## Next Steps

1. Physically inspect the SIXpack model
2. Create subtasks for each migration component
3. Make pinout decision based on inspection
4. Design dual-purpose JST connector mounting solution
5. Procure necessary components (JST connectors, Schottky diodes, power components)
6. Implement and test incrementally
