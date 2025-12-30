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

**Connector Selection: 6-pin JST Connector with Ribbon Cable**

**Implementation:**
- **On Hexapod:** 6-pin JST socket mounted on body
- **Cable:** JST connector with ribbon cable for flexibility
- **On ESP-prog:** Corresponding 6-pin JST connector

**Advantages of JST + Ribbon Cable:**
- ✅ Keyed connection - prevents reverse insertion
- ✅ Robust mechanical connection
- ✅ Easy to disconnect/reconnect for programming sessions
- ✅ Ribbon cable provides flexibility for routing
- ✅ Professional appearance
- ✅ Strain relief from JST housing

**Pin Order (Standard JST-SH 1mm pitch or JST-XH 2.5mm pitch):**
1. GND (Black)
2. VCC (Red)
3. EN (Yellow)
4. IO0 (Green)
5. RXD (Blue)
6. TXD (White)

**Key Advantage:**
Since programming interface uses UART, it's **compatible with keeping TX/RX (GPIO1/3) free** for both serial debugging and programming. I2C can remain on GPIO14/15 without affecting ESP-prog functionality.

### 3. Micro-USB Power Supply Integration

**Goal:** Add USB power as alternative to battery pack for easier development

**Requirements:**
1. Micro-USB power input connector
2. Voltage regulation:
   - USB provides 5V
   - ESP32 requires 3.3V (regulated on-board)
   - Servos typically need 5-6V (PCA9685 can handle 6V VCC)
3. Power switching mechanism:
   - Jumper or switch to select USB vs battery
   - Prevent simultaneous power sources (backfeeding)
4. Current capacity:
   - 12 servos can draw significant current (potentially 1-2A each under load)
   - USB 2.0 provides 500mA, USB 3.0 provides 900mA
   - **USB likely insufficient for full servo load**
   - USB power suitable for testing without heavy load

**Recommendation:**
- USB power for development/testing with limited servo movement
- Battery pack for full operation
- Add current limiting or warning for USB power mode

### 4. Physical Model Modifications

**Planned Additions:**
1. **ESP-prog programming interface connector**
   - Type: 6-pin JST socket (JST-XH 2.5mm or JST-SH 1mm pitch)
   - Location: Accessible side of hexapod body (TBD based on physical inspection)
   - Mounting: 3D-printed bracket or epoxy-mounted directly to body
   - Cable: Ribbon cable with JST connectors for ESP-prog connection
   - Pin assignment: GND, VCC, EN, IO0, RXD, TXD (see Phase 3 for detailed wiring)

2. **Micro-USB power connector**
   - Location: Accessible on body
   - Type: Micro-USB breakout board or panel-mount connector
   - Power switching: Automatic diode-OR circuit (no manual switch needed)

3. **Automatic power selection circuit**
   - Type: Diode-OR configuration using Schottky diodes
   - Operation: Automatically selects between USB and battery based on which is connected
   - Components:
     - Schottky diode on USB 5V input (e.g., 1N5819, low voltage drop ~0.2V)
     - Schottky diode on battery input (prevents backfeed to battery when USB connected)
     - Optional: LED indicator showing which power source is active
   - Advantages:
     - ✅ No manual switching required - plug and go
     - ✅ Automatic failover if USB unplugged
     - ✅ Backfeed protection built-in
     - ✅ Simpler user experience
     - ✅ Fewer mechanical components to fail

   **Circuit Diagram:**
   ```mermaid
   graph LR
       USB[USB 5V Input] -->|"+"| D1[Schottky Diode<br/>1N5819]
       BAT[Battery Input<br/>6V-7.4V] -->|"+"| D2[Schottky Diode<br/>1N5819]
       D1 --> VBUS[Power Bus<br/>~4.8V or ~6V]
       D2 --> VBUS
       VBUS --> ESP32[ESP32<br/>VIN]
       VBUS --> PCA[PCA9685<br/>VCC]

       USB_GND[USB GND] --> GND[Common Ground]
       BAT_GND[Battery GND] --> GND
       GND --> ESP32_GND[ESP32 GND]
       GND --> PCA_GND[PCA9685 GND]

       VBUS -.->|Optional| LED1[Green LED<br/>USB Active]
       VBUS -.->|Optional| LED2[Red LED<br/>Battery Active]

       style USB fill:#90EE90
       style BAT fill:#FFB6C1
       style VBUS fill:#FFD700
       style D1 fill:#87CEEB
       style D2 fill:#87CEEB
       style LED1 fill:#90EE90,stroke-dasharray: 5 5
       style LED2 fill:#FFB6C1,stroke-dasharray: 5 5
   ```

   - Note: When both sources connected, higher voltage takes priority (typically battery if >5.2V)
   - Voltage drop: ~0.2V across each Schottky diode (USB 5V → ~4.8V, Battery 6V → ~5.8V)

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

### Phase 3: ESP-prog Programming Interface Integration
**Selected Connector:** 6-pin JST connector with ribbon cable

Hardware installation steps:
1. Source components:
   - 6-pin JST-XH or JST-SH socket (for hexapod body mount)
   - 6-pin JST connector with pre-crimped ribbon cable
   - Matching JST plug for ESP-prog end
2. Design and print JST connector mount bracket for hexapod body
   - Should be accessible when robot is assembled
   - Position to avoid servo interference
3. Install 6-pin JST socket on hexapod
   - Mount socket to body (epoxy, screws, or 3D-printed bracket)
   - Wire socket pins to ESP32:
     - Pin 1 (GND) → ESP32 GND
     - Pin 2 (VCC) → ESP32 3.3V (optional power input)
     - Pin 3 (EN) → ESP32 EN pin
     - Pin 4 (IO0) → ESP32 IO0 pin (GPIO0)
     - Pin 5 (RXD) → ESP32 TX (GPIO1)
     - Pin 6 (TXD) → ESP32 RX (GPIO3)
4. Prepare ESP-prog cable
   - Attach JST plug to ESP-prog Program interface
   - Verify pin mapping matches hexapod socket
5. Test connection
   - Connect ribbon cable between ESP-prog and hexapod
   - Verify auto-programming functionality
   - Test serial monitor communication
6. Document connection procedure with photos and pinout diagram

### Phase 4: USB Power Integration
1. Source components:
   - Micro-USB breakout or panel-mount connector
   - 2x Schottky diodes (e.g., 1N5819 or similar, rated for 1A+)
   - Optional: Dual-color LED (red=battery, green=USB) with current-limiting resistors
2. Design and build automatic power selection circuit:
   - Diode-OR configuration:
     - USB 5V → Schottky diode → Power bus
     - Battery V+ → Schottky diode → Power bus
   - Optional LED indicators for visual feedback
3. Install micro-USB connector in body
   - Position for easy access during development
   - Secure mounting to prevent mechanical stress
4. Wire diode-OR circuit to power distribution
   - Connect power bus output to ESP32 and PCA9685 power inputs
   - Ensure proper polarity and ground connections
5. Test power selection:
   - Battery only: Verify operation
   - USB only: Verify operation with limited servo movement
   - Both connected: Verify higher voltage source takes priority
   - Measure voltage drop across diodes (~0.2V for Schottky)
6. Document power limitations and usage guidelines

### Phase 5: Testing and Validation
1. Power-on test (USB and battery)
2. I2C communication verification
3. Individual servo testing
4. Gait execution tests
5. ESP-prog debugging test
6. Long-duration stability test
7. Document final configuration

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

### Risk 2: Insufficient USB Current
**Impact:** Servos brown out or ESP32 resets under load
**Likelihood:** High - 12 servos can draw >>1A, USB provides 500-900mA
**Mitigation:**
- Current limiting circuit or fuse
- Warning LED when USB power is selected
- Clear documentation stating USB is for testing only, not full operation
- Test with progressively increasing servo load
- Battery pack required for actual robot movement

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
**Impact:** Cannot mount ESP-prog connector or USB port in body
**Likelihood:** Medium
**Mitigation:**
- Design compact mounting solutions
- Use right-angle connectors to save space
- Consider external adapter cables instead of on-board mounting
- May need to modify body STL and reprint with connector cutouts

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
4. Design ESP-prog and USB power mounting solutions
5. Procure necessary components (connectors, power components)
6. Implement and test incrementally
