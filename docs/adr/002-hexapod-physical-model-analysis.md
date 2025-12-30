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

**Connector Options:**
1. **6-pin header** (2.54mm pitch) - simple, requires jumper wires
2. **6-pin JST connector** - more robust, keyed connection
3. **Pogo pin connector** - easy attach/detach, no inserted connector

**Key Advantage:**
Since programming interface uses UART, it's **compatible with keeping TX/RX (GPIO1/3) free** for both serial debugging and programming. I2C can remain on GPIO14/15 or be moved to GPIO21/22 without affecting ESP-prog functionality.

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
1. **ESP-prog connector mount**
   - Location: TBD based on physical inspection
   - Type: 6-pin header or connector
   - Consideration: Easy attach/detach mechanism

2. **Micro-USB power connector**
   - Location: Accessible on body
   - Type: Micro-USB breakout board or panel-mount connector
   - Safety: Add protection diode to prevent backfeed

3. **Power selection mechanism**
   - Type: DPDT switch or jumper block
   - Location: Accessible but protected
   - Labels: Clear marking of USB vs Battery

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
1. Design ESP-prog connector bracket/mount
2. Select connector type (6-pin header, JST, or pogo pins)
3. 3D print or fabricate mounting solution
4. Install 6-pin programming header on robot
   - TXD → ESP32 RX (GPIO3)
   - RXD → ESP32 TX (GPIO1)
   - IO0 → ESP32 IO0 (boot mode control)
   - EN → ESP32 EN (reset control)
   - GND → Ground
   - VCC → 3.3V (optional)
5. Document connection procedure
6. Test auto-programming functionality with ESP-prog

### Phase 4: USB Power Integration
1. Source micro-USB breakout or panel-mount connector
2. Design power switching circuit
   - Schottky diode for backfeed protection
   - Optional current limit indicator LED
3. Install connector in body
4. Wire to power distribution
5. Test with limited servo load
6. Document power limitations

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
