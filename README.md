# Robot Spider

An ESP32-CAM based hexapod robot project with experimental C++ architecture.

**Current Status**: ✅ Active development - Core architecture complete, hardware testing in progress

## Overview

This is an Arduino-based hexapod robot built on the ESP32-CAM platform. The hardware design is based on the free 3D-printable "Sixpack" hexapod by delta3robotics.

* **3D Model**: https://cults3d.com/de/modell-3d/verschiedene/sixpack
* **Platform**: ESP32-CAM (FQBN: esp32:esp32:esp32cam)
* **Servo Controller**: Adafruit PWM Servo Driver (I2C at 0x40)
* **Configuration**: 6 legs with 2 servos each (12 servos total)

## Project Goals

This project is a ground-up rewrite of the robot software with several experimental objectives:

* ✅ Remove all code from .ino files (minimal setup/loop only)
* ✅ Use C++ instances instead of global status structures
* ✅ Declarative memory allocation (zero dynamic allocation)
* ✅ Include testing library for unit tests

**All goals have been achieved!**

## Quick Start

### Prerequisites (macOS)

```bash
# Install Arduino CLI
brew install arduino-cli

# USB to Serial drivers (older Macs only)
# Modern macOS (Mavericks+) has built-in support
# If needed: https://ftdichip.com

# Initialize project dependencies
make init
```

### Build and Deploy

```bash
# Build the project
make build

# Find your serial port
make usb

# Upload to device (replace with your actual port)
SERIAL_PORT=/dev/cu.usbserial-143114111 make upload

# Monitor serial output
SERIAL_PORT=/dev/cu.usbserial-143114111 make monitor
```

### Build Commands

| Command | Description |
|---------|-------------|
| `make init` | Setup board manager and install dependencies |
| `make build` | Compile the project |
| `make upload` | Upload to device (requires SERIAL_PORT) |
| `make monitor` | Open serial monitor at 9600 baud |
| `make usb` | List available USB serial ports |
| `make test` | Run unit tests |
| `make clean` | Clean build artifacts |

**Current Build Stats:**
- Program: ~332KB (10% of 3MB flash)
- RAM: ~21KB (6% of 327KB)

## Architecture

This project uses a clean, joint-centric object-oriented architecture with several key design patterns:

### Core Concepts

- **Joint-Centric Design**: Each physical joint (Shoulder, Knee) owns its servo and movement logic
- **Named Legs**: Six explicit leg classes (LeftFrontLeg, LeftMiddleLeg, etc.) for type safety and leg-specific behavior
- **Time-Based Movement**: Smooth servo control using speed (units/second) and delta time
- **Stateless Sequences**: Movement patterns described as reusable commands that can be applied repeatedly
- **Zero Dynamic Allocation**: All memory allocated declaratively on the stack at compile time

### Physical Structure

```
Robot
├── Body (owns 12 servos and 6 legs)
│   ├── LeftFrontLeg (shoulder + knee)
│   ├── LeftMiddleLeg (shoulder + knee)
│   ├── LeftRearLeg (shoulder + knee)
│   ├── RightFrontLeg (shoulder + knee)
│   ├── RightMiddleLeg (shoulder + knee)
│   └── RightRearLeg (shoulder + knee)
└── Gait Sequences (stateless movement patterns)
```

### Design Documentation

For detailed architecture documentation including:
- Complete class model with mermaid diagram
- Class descriptions and responsibilities
- Design patterns and memory management
- Future feature roadmap

**See: [ai/docs/project-analysis.md](ai/docs/project-analysis.md)**

## Current Status

### What Works ✅

- Clean object-oriented architecture with 6 named legs
- Time-based movement system for smooth servo control
- Proper encapsulation and ownership model
- Declarative memory allocation (no dynamic allocation)
- Minimal .ino file (just setup/loop delegation)
- Testing framework infrastructure
- Stateless sequence system
- Build system using arduino-cli

### Hardware Testing

**Currently Connected:**
- LeftFrontLeg shoulder servo (servo #0) - Moving in 2-second arcs

**Ready to Connect:**
- 11 additional servos (servos 1-11)

## Project Structure

```
robot-spider/
├── robot-spider.ino          # Main Arduino sketch (minimal)
├── libraries/
│   ├── robot/                # Core robot logic (primary product)
│   │   ├── robot.{h,cpp}     # Main orchestrator
│   │   ├── body.{h,cpp}      # Manages 6 legs and 12 servos
│   │   ├── joint.{h,cpp}     # Base joint with time-based movement
│   │   ├── shoulder/knee.{h,cpp}  # Joint implementations
│   │   ├── leg.{h,cpp}       # Base leg class
│   │   ├── [6 named leg classes]
│   │   ├── gait_sequence.h   # Sequence interface
│   │   └── arc_test_sequence.{h,cpp}
│   ├── logging/              # Serial logging utility
│   ├── flash/                # LED flasher for status
│   └── unit_test/            # Testing framework
├── tests/                    # Unit tests
├── ai/docs/                  # Project documentation
└── gen/                      # Build output directory
```

## Next Steps

### Immediate Goals

1. **Connect All Servos**: Wire up the remaining 11 servos to complete the hexapod
2. **Create Walking Gaits**: Implement TripodGait, WaveGait, and RippleGait sequences
3. **Add Inverse Kinematics**: Calculate joint angles from desired foot positions

### Future Features

- **Remote Control**: WiFi/Bluetooth control via web interface or mobile app
- **Camera Integration**: Enable ESP32-CAM for video streaming and computer vision
- **Sensor Integration**: Distance sensors, IMU for balance, battery monitoring
- **Autonomous Behaviors**: Obstacle avoidance, patrol mode, terrain adaptation
- **Safety Features**: Emergency stop, servo limits, current limiting

See [ai/docs/project-analysis.md](ai/docs/project-analysis.md) for the complete feature roadmap.

## Hardware Configuration

- **Board**: ESP32-CAM
- **PWM Driver**: Adafruit PWM Servo Driver (I2C at 0x40, 60Hz)
- **I2C Pins**: SDA=15, SCL=14
- **LED Pin**: GPIO 33
- **Serial**: 9600 baud, 8N1, DTR/RTS enabled
- **Servo Range**: 150-600 PWM (middle: 375)
- **Servo Mapping**: 0-11 (LeftFront, LeftMiddle, LeftRear, RightFront, RightMiddle, RightRear)

## Contributing

This is an experimental project focused on exploring C++ design patterns in embedded systems. The codebase prioritizes:

- Clean architecture over quick hacks
- Compile-time safety over runtime flexibility
- Explicit ownership over implicit behavior
- Readable code over clever code

## License

This project uses the free "Sixpack" hexapod hardware design by delta3robotics. Please respect the original hardware design license when using or modifying this project.

---

**Last Updated**: 2025-11-15
**Platform**: ESP32-CAM
**Build System**: arduino-cli + Makefile

