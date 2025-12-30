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

#### Issue #1: I2C Pin Conflict
**Problem:** Physical SIXpack model may have hardwired connections to GPIO1/3

**Impact:**
- If hardware is fixed to GPIO1/3: Must modify robot-spider firmware
- If hardware is configurable: Can keep GPIO14/15 (preferred)

**Decision Needed:** Inspect physical model to determine if I2C pins are hardwired

**Options:**
1. **Keep GPIO14/15 (Recommended)**
   - Requires checking if physical model has flexible wiring
   - Maintains serial debugging capability
   - Better development experience

2. **Switch to GPIO1/3 (If Required)**
   - Matches original SIXpack design
   - Loses serial debugging when I2C active
   - May require hardware modification if pins are fixed

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

**Goal:** Enable in-place debugging while robot is mounted on physical model

**Requirements:**
1. ESP-prog connector placement
   - Must not interfere with servo operation
   - Should be accessible when robot is assembled
   - May need custom mounting bracket

2. Required JTAG Pins:
   - TMS: GPIO14
   - TCK: GPIO13
   - TDI: GPIO12
   - TDO: GPIO15
   - GND: Ground
   - VCC: 3.3V

**CONFLICT DETECTED:**
- GPIO14 is currently used for I2C_SCL
- GPIO15 is currently used for I2C_SDA
- **Cannot use JTAG and current I2C pins simultaneously**

**Resolution Options:**
1. Move I2C to different pins (e.g., GPIO21/22) to free up GPIO14/15 for JTAG
2. Use ESP-prog only when needed (disconnect I2C temporarily)
3. Use alternative debugging methods (serial logging, WiFi debugging)

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

### Phase 2: Pinout Decision
Based on physical inspection:
- **If I2C is hardwired to GPIO1/3:**
  - Create subtask to migrate firmware to GPIO1/3
  - Document serial debugging workaround
  - Consider adding I2C level shifter for flexibility

- **If I2C wiring is flexible:**
  - Keep GPIO14/15 for I2C
  - Update physical wiring to match
  - Move I2C pins again if JTAG debugging needed (to GPIO21/22)

### Phase 3: ESP-prog Integration
1. Select final I2C pins to avoid JTAG conflict
2. Design ESP-prog connector bracket
3. 3D print or fabricate mounting solution
4. Add 6-pin header to robot
5. Document connection procedure
6. Test JTAG debugging functionality

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

## Pin Migration Decision Matrix

| Scenario | I2C Pins | JTAG Available | Serial Debug | Action Required |
|----------|----------|----------------|--------------|-----------------|
| A: Hardwired GPIO1/3 | GPIO1/3 | No (conflicts) | Limited | Accept limitations, use WiFi debug |
| B: Flexible wiring + No JTAG | GPIO14/15 | No (conflicts) | Yes | Keep current config |
| C: Flexible wiring + JTAG needed | GPIO21/22 | Yes | Yes | Migrate I2C, update wiring |

**Recommendation:** Pursue Scenario C if JTAG debugging is desired, otherwise Scenario B is optimal.

## Risks and Mitigations

### Risk 1: Hardwired I2C Pins
**Impact:** Forced to use GPIO1/3, losing serial debugging
**Mitigation:**
- Use WiFi-based logging instead
- Add removable jumpers for I2C to enable serial when needed
- Consider hardware modification to rewire I2C

### Risk 2: Insufficient USB Current
**Impact:** Servos brown out or ESP32 resets under load
**Mitigation:**
- Current limiting circuit
- Warning LED when USB power is selected
- Documentation clearly stating USB limitations
- Test with progressively increasing servo load

### Risk 3: JTAG/I2C Pin Conflict
**Impact:** Cannot use ESP-prog with current pin configuration
**Mitigation:**
- Move I2C to GPIO21/22
- Use ESP-prog only during specific debugging sessions
- Rely on serial/WiFi debugging instead

### Risk 4: Physical Space Constraints
**Impact:** Cannot mount ESP-prog connector or USB port
**Mitigation:**
- Design compact mounting solutions
- Use right-angle connectors
- Consider external adapter cables instead of on-board mounting

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
