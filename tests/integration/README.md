# Bluetooth Integration Tests

Python-based integration tests for RobotSpider Bluetooth communication using PyBluez.

## Overview

These tests verify Bluetooth Classic (SPP) connectivity between a development machine and the RobotSpider ESP32-CAM device. The tests use Python with PyBluez for cross-platform Bluetooth support.

## Prerequisites

**Hardware:**
- RobotSpider hexapod with ESP32-CAM (powered on)
- Development machine with Bluetooth capability

**Software:**
- Python 3.6 or higher
- pip3 (Python package manager)
- Bluetooth enabled on development machine
- Platform-specific Bluetooth libraries (see below)

## Platform-Specific Requirements

### macOS

```bash
# Xcode Command Line Tools (required for compiling native extensions)
xcode-select --install

# PyBluez should work out of the box on macOS
# Bluetooth must be enabled in System Settings
```

### Linux (Ubuntu/Debian)

```bash
# Install Bluetooth development libraries
sudo apt-get update
sudo apt-get install -y \
    libbluetooth-dev \
    bluetooth \
    bluez

# Ensure Bluetooth service is running
sudo systemctl start bluetooth
sudo systemctl enable bluetooth

# Add your user to the bluetooth group (may require logout/login)
sudo usermod -a -G bluetooth $USER
```

### Linux (Fedora/RHEL)

```bash
# Install Bluetooth development libraries
sudo dnf install -y \
    bluez-libs-devel \
    bluez

# Start Bluetooth service
sudo systemctl start bluetooth
sudo systemctl enable bluetooth
```

### Windows

```bash
# Install Microsoft C++ Build Tools
# Download from: https://visualstudio.microsoft.com/visual-cpp-build-tools/

# PyBluez should work with Windows Bluetooth stack
# Ensure Bluetooth is enabled in Windows Settings
```

## Installation

### Quick Start

```bash
# From project root
make test-integration-install
```

### Manual Installation

```bash
cd tests/integration
pip3 install -r requirements.txt
```

### Virtual Environment (Recommended)

```bash
cd tests/integration

# Create virtual environment
python3 -m venv venv

# Activate virtual environment
# On macOS/Linux:
source venv/bin/activate
# On Windows:
venv\Scripts\activate

# Install dependencies
pip3 install -r requirements.txt
```

## Usage

### Discover and Connect to RobotSpider

```bash
# From project root
make test-integration

# Or directly
cd tests/integration
python3 test_bluetooth.py
```

### Expected Output

```
============================================================
  RobotSpider Bluetooth Integration Test
============================================================

ℹ Scanning for Bluetooth devices... (duration: 10s)
ℹ Looking for device: "RobotSpider"

Found 3 device(s):
  • iPhone (XX:XX:XX:XX:XX:XX)
  • RobotSpider (YY:YY:YY:YY:YY:YY)
  • Keyboard (ZZ:ZZ:ZZ:ZZ:ZZ:ZZ)

✓ Found "RobotSpider"!
ℹ Address: YY:YY:YY:YY:YY:YY

ℹ Searching for SPP service...
✓ Found SPP service: Serial Port
ℹ Port: 1
ℹ Protocol: RFCOMM

ℹ Connecting to RobotSpider...
ℹ Address: YY:YY:YY:YY:YY:YY
ℹ Port: 1
✓ Connected successfully!

ℹ Testing connection (keeping open for 5 seconds)...
✓ Connection stable

============================================================
  ✓ Test Completed Successfully
============================================================

✓ Connection established and tested
ℹ RobotSpider is ready to receive commands via Bluetooth

ℹ Disconnected
```

## Test Script Features

**Current Implementation:**
- ✅ Bluetooth device discovery
- ✅ Device name filtering ("RobotSpider")
- ✅ SPP service discovery
- ✅ Connection establishment
- ✅ Connection stability test
- ✅ Graceful disconnect

**Future Enhancements:**
- ⏳ Command sending (`--send` flag)
- ⏳ Response validation
- ⏳ Command sequence testing
- ⏳ Timing and latency measurements

## Troubleshooting

### PyBluez Installation Fails

**Symptoms:**
```
error: command 'gcc' failed with exit status 1
```

**Solutions:**

**macOS:**
```bash
xcode-select --install
```

**Linux:**
```bash
sudo apt-get install libbluetooth-dev
```

**Windows:**
Install Microsoft C++ Build Tools

### Device Not Found

**Symptoms:**
```
✗ Device "RobotSpider" not found
```

**Solutions:**
1. Ensure RobotSpider is powered on
2. Check ESP32 serial output for "Bluetooth initialized successfully"
3. Verify Bluetooth is enabled on development machine
4. Move closer to the device (Bluetooth Classic range ~10-30m)
5. Try power cycling the RobotSpider
6. Check if device name matches exactly ("RobotSpider")

### No Services Found

**Symptoms:**
```
⚠ No services found on device
```

**Solutions:**
1. Wait a few seconds and try again (services may not be immediately available)
2. Ensure ESP32 BluetoothSerial is properly initialized
3. Check ESP32 logs for initialization errors
4. Try unpairing and re-pairing the device

### Connection Failed

**Symptoms:**
```
✗ Connection failed: [Errno 16] Device or resource busy
```

**Solutions:**
1. Close any other app connected to RobotSpider
2. Unpair device in system Bluetooth settings, then re-pair
3. Restart Bluetooth on both devices:
   ```bash
   # macOS
   sudo killall bluetoothd

   # Linux
   sudo systemctl restart bluetooth
   ```
4. Power cycle the RobotSpider

### Permission Denied (Linux)

**Symptoms:**
```
bluetooth.BluetoothError: (13, 'Permission denied')
```

**Solutions:**
```bash
# Add user to bluetooth group
sudo usermod -a -G bluetooth $USER

# Log out and log back in, or use:
newgrp bluetooth

# Or run with sudo (not recommended for regular use)
sudo python3 test_bluetooth.py
```

## Architecture

```
┌─────────────────────────────────────────────┐
│ Development Machine (Python)                │
│                                             │
│  ┌──────────────────────────────────┐      │
│  │ test_bluetooth.py                │      │
│  │ - Device discovery               │      │
│  │ - SPP service detection          │      │
│  │ - Connection management          │──────┼─── Bluetooth Classic (SPP)
│  │ - Message handling (future)      │      │
│  └──────────────────────────────────┘      │
│                                             │
│  Dependencies:                              │
│  - PyBluez 0.23                            │
└─────────────────────────────────────────────┘
                    │
                    │ Bluetooth
                    │
┌─────────────────────────────────────────────┐
│ RobotSpider ESP32-CAM                       │
│                                             │
│  ┌──────────────────────────────────┐      │
│  │ BluetoothConnection              │      │
│  │ - Device name: "RobotSpider"     │      │
│  │ - SPP server                     │      │
│  │ - Message reception              │      │
│  └──────────────────────────────────┘      │
│                                             │
└─────────────────────────────────────────────┘
```

## Command Protocol

Commands are newline-delimited strings:

```
init\n       # Initialize robot
forward\n    # Move forward
backward\n   # Move backward
left\n       # Turn left
right\n      # Turn right
stop\n       # Stop movement
```

Expected responses (from ESP32):
```
OK: Initialized\n
OK: Moving forward\n
OK: Stopped\n
```

## Why Python + PyBluez?

**Advantages:**
- ✅ Supports Bluetooth Classic (SPP/RFCOMM) - exactly what ESP32 BluetoothSerial uses
- ✅ Cross-platform: macOS, Linux, Windows
- ✅ Simple scripting language - perfect for integration tests
- ✅ Well-documented with examples
- ✅ Easy CI/CD integration
- ✅ No dependency on deprecated Node.js packages

**Comparison to Alternatives:**
- **Node.js bluetooth-serial-port**: Deprecated, doesn't work with Node 13+, macOS support dropped in v3
- **Python PyBluez**: Functional, cross-platform, supports Bluetooth Classic
- **Other Node.js BLE libraries**: Only support BLE, not Bluetooth Classic SPP

## Related Documentation

- **ADR 003:** Bluetooth Communication Architecture (`docs/adr/003-bluetooth-communication-architecture.md`)
- **BluetoothConnection:** `libraries/robot-bluetooth/bluetooth_connection.h`
- **CommandRouter:** `libraries/robot-bluetooth/command_router.h`
- **Android App Commands:** `/robot-spider-control/lib/models/robot_command.dart`
- **PyBluez Documentation:** https://pybluez.readthedocs.io/
- **PyBluez GitHub:** https://github.com/pybluez/pybluez

## Future Enhancements

### Command Sending

```python
# Future implementation
def send_command(sock, command):
    """Send command to RobotSpider"""
    message = f"{command}\n"
    sock.send(message.encode('utf-8'))
    print(f"Sent: {command}")

    # Wait for response
    response = sock.recv(1024).decode('utf-8').strip()
    print(f"Received: {response}")
    return response

# Test commands
send_command(sock, "init")
send_command(sock, "forward")
time.sleep(2)
send_command(sock, "stop")
```

### Automated Test Suite

Future scripts could include:
- `test_commands.py` - Test all command responses
- `test_timing.py` - Measure command latency
- `test_stress.py` - Send rapid commands to test buffer handling
