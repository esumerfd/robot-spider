# RobotSpider Tests

Comprehensive test suite for the hexapod robot project.

## Test Organization

```
tests/
├── unit/              # C++ unit tests for components
│   ├── joint_test.h
│   ├── mock_servo.h
│   └── ...
│
└── integration/       # JavaScript Bluetooth integration tests
    ├── index.js
    ├── package.json
    └── README.md
```

## Test Types

### Unit Tests (`unit/`)

**Technology:** C++ with custom test framework
**Purpose:** Test individual components in isolation
**Dependencies:** None (uses mocks)
**Environment:** Development machine or Arduino board

Tests verify:
- Joint movement calculations
- Servo position interpolation
- Timing and speed logic
- Component interfaces

**Quick Start:**
```bash
cd unit/
make
```

See [unit/README.md](unit/README.md) for details.

### Integration Tests (`integration/`)

**Technology:** Node.js/JavaScript
**Purpose:** Test Bluetooth communication with real hardware
**Dependencies:** bluetooth-serial-port npm package
**Environment:** Development machine + powered RobotSpider

Tests verify:
- Bluetooth device discovery
- SPP connection establishment
- Command transmission (future)
- Message protocol (future)

**Quick Start:**
```bash
cd integration/
npm install
npm test
```

See [integration/README.md](integration/README.md) for details.

## Running All Tests

### Unit Tests Only
```bash
make -C tests/unit
```

### Integration Tests Only
```bash
cd tests/integration && npm test
```

### Full Test Suite (Future)
```bash
# Will run both unit and integration tests
npm run test:all
```

## Test Philosophy

### Unit Tests
- Fast execution (no hardware)
- Test in isolation with mocks
- Verify algorithms and logic
- Run on every code change

### Integration Tests
- Slower execution (requires hardware)
- Test real system interactions
- Verify communication protocols
- Run before deployment

## CI/CD Considerations

### Unit Tests
- ✅ Can run in CI pipeline
- ✅ No special hardware required
- ✅ Consistent, repeatable results

### Integration Tests
- ⚠️ Requires physical RobotSpider
- ⚠️ Needs Bluetooth hardware on CI runner
- 💡 Best run locally before commits
- 💡 Could use hardware-in-the-loop setup

## Adding New Tests

**For C++ component testing:**
- Add to `tests/unit/`
- Follow existing test structure
- Use mock objects for dependencies

**For system/communication testing:**
- Add to `tests/integration/`
- Use Node.js for flexibility
- Document hardware requirements

## Related Documentation

- **ADR 003:** Bluetooth Communication Architecture
- **BluetoothConnection:** `libraries/robot-bluetooth/`
- **CommandRouter:** `libraries/robot-bluetooth/`
- **Android App:** `/robot-spider-control/`

## Future Test Coverage

Planned test additions:
- [ ] Gait sequence tests (unit)
- [ ] Body coordination tests (unit)
- [ ] Command timing tests (integration)
- [ ] Stress testing (integration)
- [ ] WebSocket communication tests (integration)
- [ ] End-to-end Android app tests
