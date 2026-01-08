# Bluetooth Integration Tests

Integration tests for RobotSpider Bluetooth communication using Node.js.

## Overview

These tests verify Bluetooth Classic (SPP) connectivity between a development machine and the RobotSpider ESP32-CAM device. The tests use JavaScript/Node.js for simplicity and cross-platform compatibility.

## Prerequisites

**Hardware:**
- RobotSpider hexapod with ESP32-CAM (powered on)
- Development machine with Bluetooth capability

**Software:**
- Node.js 16+ (with ES modules support)
- Bluetooth enabled on development machine
- Python 2.7 (required for `bluetooth-serial-port` native module compilation)

**Platform-Specific Requirements:**

### macOS
```bash
# Xcode Command Line Tools (if not already installed)
xcode-select --install
```

### Linux
```bash
# Bluetooth development libraries
sudo apt-get install bluetooth bluez libbluetooth-dev
```

### Windows
- Windows 10/11 with Bluetooth support
- Visual Studio Build Tools (for native module compilation)

## Installation

```bash
cd tests/integration
npm install
```

**Note:** The `bluetooth-serial-port` package includes native modules that need to be compiled during installation. This may take a few minutes.

## Usage

### Discover and Pair with RobotSpider

```bash
npm test
```

This will:
1. Scan for Bluetooth devices
2. Locate the "RobotSpider" device
3. Find the SPP (Serial Port Profile) channel
4. Establish connection
5. Listen for incoming messages
6. Disconnect after 5 seconds

### Expected Output

```
═══════════════════════════════════════════════
  RobotSpider Bluetooth Integration Test
═══════════════════════════════════════════════

🔍 Scanning for Bluetooth devices...
   Looking for device: "RobotSpider"

   Found: RobotSpider (XX:XX:XX:XX:XX:XX)

✅ Found "RobotSpider"!
   Address: XX:XX:XX:XX:XX:XX

🔌 Checking available SPP channels...
   ✅ Found SPP channel: 1

🔗 Connecting to RobotSpider...
   Address: XX:XX:XX:XX:XX:XX
   Channel: 1
   ✅ Connected successfully!

═══════════════════════════════════════════════
  ✅ Test Completed Successfully
═══════════════════════════════════════════════

💡 Connection established and ready for commands
   (Command sending will be implemented next)

⏱️  Keeping connection open for 5 seconds...
👋 Disconnected
```

## Architecture

```
┌─────────────────────────────────────────────┐
│ Development Machine (Node.js)              │
│                                             │
│  ┌──────────────────────────────────┐      │
│  │ index.js                         │      │
│  │ - Device discovery               │      │
│  │ - SPP channel detection          │      │
│  │ - Connection management          │      │
│  │ - Message handling (future)      │──────┼─── Bluetooth Classic (SPP)
│  └──────────────────────────────────┘      │
│                                             │
│  Dependencies:                              │
│  - bluetooth-serial-port                    │
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

## Troubleshooting

### Device Not Found

**Symptoms:**
```
❌ Device "RobotSpider" not found
```

**Solutions:**
1. Ensure RobotSpider is powered on
2. Check ESP32 serial output for "Bluetooth initialized successfully"
3. Verify Bluetooth is enabled on development machine
4. Move closer to the device (Bluetooth Classic range ~10m)
5. Restart Bluetooth on development machine
6. Power cycle the RobotSpider

### No SPP Channel Found

**Symptoms:**
```
❌ No SPP channel found
```

**Solutions:**
1. Verify ESP32 firmware is running Bluetooth Classic (not BLE)
2. Check that BluetoothSerial was initialized in Robot::setup()
3. Review ESP32 serial logs for initialization errors

### Connection Failed

**Symptoms:**
```
❌ Connection failed
```

**Solutions:**
1. Try unpairing and re-pairing the device in system Bluetooth settings
2. Ensure no other application is connected to RobotSpider
3. Check ESP32 accepts incoming connections (no PIN required by default)
4. Verify SPP channel number is correct

### Native Module Compilation Errors

**Symptoms:**
```
npm install fails with compiler errors
```

**Solutions:**
- **macOS:** Install Xcode Command Line Tools
- **Linux:** Install bluetooth development libraries
- **Windows:** Install Visual Studio Build Tools
- Try using a Node.js LTS version (16 or 18)

## Future Enhancements

### Command Sending (Next Phase)

The integration test will be extended to send commands:

```javascript
// Future implementation
async function sendCommand(connection, command) {
  console.log(`📤 Sending: ${command}`);
  connection.write(Buffer.from(command + '\n', 'utf-8'));

  // Wait for acknowledgment
  const response = await waitForResponse(connection);
  console.log(`📨 Received: ${response}`);
}

// Test commands
await sendCommand(connection, 'init');
await sendCommand(connection, 'forward');
await sendCommand(connection, 'stop');
```

### Automated Test Suite

```bash
npm run test:commands    # Test all command responses
npm run test:timing      # Test command timing and latency
npm run test:stress      # Send rapid commands to test buffer handling
```

## Related Documentation

- **ADR 003:** Bluetooth Communication Architecture (`docs/adr/003-bluetooth-communication-architecture.md`)
- **BluetoothConnection:** `libraries/robot-bluetooth/bluetooth_connection.h`
- **CommandRouter:** `libraries/robot-bluetooth/command_router.h`
- **Android App Commands:** `/robot-spider-control/lib/models/robot_command.dart`

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

Expected responses:
```
OK: Initialized\n
OK: Moving forward\n
OK: Stopped\n
```
