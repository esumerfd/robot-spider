# ADR 003: Bluetooth Communication Architecture

## Status
Proposed

## Date
2026-01-07

## Context

The hexapod robot currently supports WebSocket-based communication over WiFi for receiving control commands from the Android app. While this works well for network-connected scenarios, it has limitations:

- Requires the robot and phone to be on the same WiFi network
- Adds complexity with mDNS discovery and IP address management
- WiFi has higher power consumption compared to Bluetooth
- Network setup can be cumbersome for quick operation

The Android app (robot-spider-control) currently defines the following command interface in `lib/models/robot_command.dart`:
- `init` - Initialize robot state
- `forward` - Move forward
- `backward` - Move backward
- `left` - Turn left
- `right` - Turn right

These commands are transmitted as string messages over WebSocket. To enable direct phone-to-robot pairing without network infrastructure, we need to add Bluetooth support as an alternative communication method.

## Requirements

1. **Command Compatibility**: Maintain the same command interface (`init`, `forward`, `backward`, `left`, `right`) used by the Android app
2. **Message Isolation**: Isolate the Bluetooth communication layer from the command processing logic
3. **Callback Architecture**: Provide a callback registration mechanism for handling incoming messages
4. **Coexistence**: Support both WebSocket and Bluetooth simultaneously (allowing either connection method)
5. **Low Complexity**: Keep the implementation simple and maintainable
6. **ESP32 Compatibility**: Work with the ESP32-CAM hardware platform

## Investigation

### ESP32 Bluetooth Options

The ESP32 supports two Bluetooth technologies:

1. **Bluetooth Classic (BR/EDR)**
   - Serial Port Profile (SPP) - provides UART-like interface
   - Higher power consumption than BLE
   - Better range and throughput
   - Simpler API for command/response patterns
   - Well-supported by Android BluetoothSocket

2. **Bluetooth Low Energy (BLE)**
   - Lower power consumption
   - More complex GATT characteristic model
   - Better for periodic sensor data
   - Requires more code for characteristic definitions
   - Good Android support via BluetoothGatt

### Library Options

1. **BluetoothSerial** (Classic SPP)
   - Built into ESP32 Arduino core
   - Simple Serial-like API
   - Example: `BluetoothSerial SerialBT;`
   - Easy integration with existing string-based commands

2. **BLE Libraries** (NimBLE-Arduino, ESP32 BLE Arduino)
   - More complex setup with services/characteristics
   - Better power efficiency
   - Requires defining GATT profile

### Android App Compatibility

Current implementation uses:
- `RobotConnectionProvider` for managing connection state
- `WebSocketService` for network communication
- String-based command transmission

For Bluetooth support, the Android app would need:
- `BluetoothService` (similar to WebSocketService)
- Same `RobotCommand` enum mapping to strings
- Bluetooth device discovery and pairing UI

## Decision

**Implement Bluetooth Classic (SPP) using the ESP32 BluetoothSerial library** for the following reasons:

1. **API Simplicity**: BluetoothSerial provides a Serial-like interface that matches our string command model
2. **Protocol Compatibility**: Can reuse the same string-based command format as WebSocket
3. **Android Simplicity**: BluetoothSocket on Android provides a simple InputStream/OutputStream model
4. **Range and Reliability**: Better suited for real-time control commands than BLE
5. **Development Speed**: Faster to implement and test than BLE GATT profile

### Architecture Design

```
┌─────────────────────────────────────────────────────┐
│ ESP32 Hexapod Robot                                 │
│                                                     │
│  ┌──────────────┐        ┌────────────────────┐   │
│  │ WebSocket    │        │ Bluetooth Classic  │   │
│  │ Server       │        │ (SPP)              │   │
│  └──────┬───────┘        └─────────┬──────────┘   │
│         │                          │              │
│         └────────┬─────────────────┘              │
│                  │                                │
│         ┌────────▼─────────────┐                 │
│         │ Command Router       │                 │
│         │ (Message Callbacks)  │                 │
│         └────────┬─────────────┘                 │
│                  │                                │
│         ┌────────▼─────────────┐                 │
│         │ Command Processor    │                 │
│         │ - parseCommand()     │                 │
│         │ - executeCommand()   │                 │
│         └────────┬─────────────┘                 │
│                  │                                │
│         ┌────────▼─────────────┐                 │
│         │ Robot Controller     │                 │
│         │ (existing code)      │                 │
│         └──────────────────────┘                 │
└─────────────────────────────────────────────────────┘
```

### Implementation Components

1. **BluetoothConnection Class** (`lib/robot-bluetooth/bluetooth_connection.h|cpp`)
   - Wraps BluetoothSerial library
   - Manages device name and pairing
   - Reads incoming string commands
   - Provides callback registration: `onMessageReceived(std::function<void(String)>)`

2. **CommandRouter Class** (`lib/robot-bluetooth/command_router.h|cpp`)
   - Registers callbacks from both WebSocket and Bluetooth
   - Parses command strings ("forward", "backward", etc.)
   - Routes to appropriate handler function
   - Maintains command→handler mapping

3. **Robot Integration** (update `robot.cpp`)
   - Initialize both communication methods in `Robot::setup()`
   - Register command callbacks with CommandRouter
   - Execute movement commands via existing Body/Gait classes

### Message Protocol

Maintain simple newline-delimited string protocol:
```
Command Format: "<command>\n"
Examples:
  "init\n"
  "forward\n"
  "backward\n"
  "left\n"
  "right\n"
```

This matches the existing WebSocket message format for consistency.

## Consequences

### Positive

1. **Direct Pairing**: Users can connect phone directly to robot without WiFi network
2. **Lower Barrier to Entry**: No network configuration required for basic operation
3. **Code Reuse**: Same command processing logic for both communication methods
4. **Flexibility**: Users can choose WebSocket or Bluetooth based on their needs
5. **Simple Testing**: Can test commands via Serial Bluetooth Terminal apps

### Negative

1. **Increased Code Size**: Additional Bluetooth library and communication code
2. **Power Consumption**: Running both WiFi and Bluetooth increases power usage (can be mitigated by enabling only one at a time)
3. **Android App Changes**: Requires updating the robot-spider-control app to support Bluetooth
4. **Range Limitation**: Bluetooth Classic typically 10-30m range vs WiFi's longer range
5. **Single Client**: Bluetooth SPP supports one connection at a time (vs WebSocket's potential for multiple clients)

### Security Considerations

1. **Pairing PIN**: Implement PIN-based pairing for Bluetooth connection
2. **Command Validation**: Validate all incoming commands before execution
3. **Connection State**: Only process commands when properly connected and authenticated

### Testing Strategy

1. **Unit Tests**:
   - Test command parsing with valid/invalid inputs
   - Test callback registration and invocation
   - Test message routing logic

2. **Integration Tests**:
   - Test BluetoothSerial message reception
   - Test concurrent WebSocket and Bluetooth connections
   - Test graceful handling of disconnections

3. **Manual Tests**:
   - Pair with Android phone and send commands
   - Test range and reliability
   - Test battery impact with both methods active

## Alternatives Considered

### BLE with Custom GATT Profile

**Rejected** because:
- More complex implementation (service/characteristic definitions)
- Android app would need significantly more code
- Overkill for simple command transmission
- Better suited for sensor data streaming, not control commands

### Bluetooth Classic with Custom Protocol

**Rejected** because:
- No benefit over simple string commands
- Would complicate Android app parsing
- Current string format is human-readable and debuggable

### WiFi Direct

**Rejected** because:
- Less mature library support on ESP32
- More complex setup than Bluetooth
- Android WiFi Direct API is more complex than Bluetooth

## Implementation Plan

1. Create `lib/robot-bluetooth/` directory structure
2. Create BluetoothConnection class with callback support (`lib/robot-bluetooth/bluetooth_connection.h|cpp`)
3. Create CommandRouter class for message routing (`lib/robot-bluetooth/command_router.h|cpp`)
4. Refactor existing WebSocket command handling to use CommandRouter
5. Integrate Bluetooth initialization in Robot::setup()
6. Add configuration option to enable/disable each communication method
7. Update Android app to support Bluetooth connection
8. Test both communication methods independently and concurrently

## References

- ESP32 BluetoothSerial Library: https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial
- Android BluetoothSocket Documentation: https://developer.android.com/reference/android/bluetooth/BluetoothSocket
- Current Android app command interface: `/robot-spider-control/lib/models/robot_command.dart`
- Current WebSocket implementation: `/robot-spider-control/lib/services/websocket_service.dart`

## Related ADRs

- ADR 001: Servo PWM Initialization Thread Safety (relevant for concurrent connection handling)
- ADR 002: Hexapod Physical Model Analysis (defines the robot capabilities being controlled)
