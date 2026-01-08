# Testing Guide

Comprehensive guide for running tests on the RobotSpider hexapod project.

## Quick Start

```bash
# Build unit tests only (no hardware required)
make test-unit

# Run Bluetooth integration tests (requires RobotSpider powered on)
make test-integration

# Run all tests (builds unit tests, runs integration tests)
make test
```

## Test Types

### Unit Tests (C++)

**Location:** `tests/robot-tests/`
**Technology:** C++ with Arduino framework
**Purpose:** Test component logic in isolation

**Make Targets:**
```bash
make test-unit                    # Build unit tests (compile only)
cd tests/robot-tests && make run  # Build, upload to board, and monitor results
```

**Requirements:**
- None for building
- ESP32 board + serial port for running on hardware

**What's Tested:**
- Joint movement calculations
- Servo position interpolation
- Timing and speed logic

### Integration Tests (JavaScript)

**Location:** `tests/integration/`
**Technology:** Node.js with bluetooth-serial-port
**Purpose:** Test Bluetooth communication with real hardware

**Make Targets:**
```bash
make test-integration-install   # Install npm dependencies
make test-integration           # Run Bluetooth pairing test
```

**Direct npm usage:**
```bash
cd tests/integration
npm install
npm test
```

**Requirements:**
- Node.js 16+
- Bluetooth enabled on development machine
- RobotSpider powered on with Bluetooth running
- Platform-specific requirements (see tests/integration/README.md)

**What's Tested:**
- Bluetooth device discovery
- SPP connection establishment
- Message reception
- (Future: Command sending and response validation)

## Makefile Targets Reference

### Main Makefile

| Target | Description |
|--------|-------------|
| `make test` | Run all tests (unit build + integration) |
| `make test-unit` | Build C++ unit tests |
| `make test-integration` | Run Bluetooth integration tests |
| `make test-integration-install` | Install integration test dependencies |

### Unit Test Makefile (`tests/robot-tests/Makefile`)

| Target | Description |
|--------|-------------|
| `make` or `make build` | Compile unit tests (no upload) |
| `make test` | Same as build (compile only) |
| `make run` | Build, upload to ESP32, and monitor serial output |
| `make clean` | Remove build artifacts |

**Running on hardware:**
```bash
cd tests/robot-tests
SERIAL_PORT=/dev/cu.usbserial-0001 make run
```

## CI/CD Considerations

### Automated Testing

**Unit Tests:**
- ✅ Can build in CI pipeline
- ⚠️ Running requires ESP32 hardware (skip upload/monitor in CI)
- Use `make test-unit` for CI (builds only)

**Integration Tests:**
- ⚠️ Requires physical RobotSpider with Bluetooth
- ⚠️ Requires Bluetooth hardware on CI runner
- Best run locally before commits
- Consider hardware-in-the-loop setup for automated testing

### Recommended CI Pipeline

```yaml
# .github/workflows/test.yml (example)
name: Test

on: [push, pull_request]

jobs:
  unit-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Setup Arduino CLI
        run: |
          # Install arduino-cli
      - name: Build unit tests
        run: make test-unit

  # Integration tests run manually or on dedicated hardware runner
```

## Test Development Workflow

### Adding Unit Tests

1. Create test file in `tests/robot-tests/`
2. Include `unit_test.h` framework
3. Write test cases with `TEST()` macro
4. Add to `main.cpp` if needed
5. Run `make test-unit` to verify it compiles

### Adding Integration Tests

1. Create test script in `tests/integration/`
2. Add to package.json scripts if needed
3. Document hardware requirements
4. Test with real RobotSpider hardware
5. Update README with usage instructions

## Troubleshooting

### Unit Tests

**Build fails:**
- Check Arduino CLI is installed: `arduino-cli version`
- Verify ESP32 core is installed: `arduino-cli core list`
- Check library paths in Makefile

**Upload fails:**
- Verify serial port: `make usb`
- Check permissions: `ls -l /dev/cu.*` or `/dev/ttyUSB*`
- Try different USB cable/port
- Press reset button on ESP32 before upload

### Integration Tests

**npm install fails:**
- Check Node.js version: `node --version` (need 16+)
- Install platform dependencies (see tests/integration/README.md)
- Try clearing cache: `npm cache clean --force`

**Device not found:**
- Verify RobotSpider is powered on
- Check Bluetooth is enabled on dev machine
- Review ESP32 serial output for "Bluetooth initialized"
- Try `bluetoothctl` (Linux) or Bluetooth preferences (macOS)

**Connection fails:**
- Unpair device in system settings, then retry
- Verify no other app is connected to RobotSpider
- Check ESP32 logs for connection errors
- Restart Bluetooth on both devices

## Related Documentation

- **tests/README.md** - Test organization overview
- **tests/robot-tests/README.md** - Unit test details
- **tests/integration/README.md** - Integration test details with troubleshooting
- **ADR 003** - Bluetooth Communication Architecture
- **Main README.md** - Project setup and build instructions

## Future Enhancements

Planned testing improvements:

- [ ] Command sending in integration tests
- [ ] Automated command response validation
- [ ] Stress testing (rapid command sequences)
- [ ] Latency measurements
- [ ] WebSocket integration tests
- [ ] End-to-end tests with Android app
- [ ] Mock Bluetooth for unit testing command router
- [ ] Hardware-in-the-loop CI setup
