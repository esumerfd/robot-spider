# Unit Tests

C++ unit tests for RobotSpider components.

## Overview

These tests verify individual components of the hexapod robot in isolation using mock objects. Tests run on the development machine without requiring ESP32 hardware.

## Structure

```
tests/robot-tests/
├── README.md           # This file
├── Makefile           # Build configuration for unit tests
├── robot-tests.ino    # Arduino sketch wrapper for tests
├── main.cpp           # Test runner entry point
├── joint_test.h       # Joint movement and timing tests
└── mock_servo.h       # Mock Servo class for testing
```

## Running Tests

### Arduino IDE

1. Open `robot-tests.ino` in Arduino IDE
2. Select your board (tests run on any board)
3. Upload and run
4. Open Serial Monitor to view test results

### Command Line (Make)

```bash
cd tests/robot-tests
make
```

## Test Components

### Joint Tests (`joint_test.h`)

Tests for the `Joint` class movement and timing logic:
- Time-based position interpolation
- Speed calculations (units per second)
- Target position reaching
- Edge cases (zero delta time, large time steps)

Uses `MockServo` to avoid hardware dependencies.

### Mock Objects (`mock_servo.h`)

Mock implementations for testing:
- `MockServo` - Simulates Servo without PWM hardware
- Records position changes for verification
- Allows testing movement logic in isolation

## Test Framework

Uses custom `unit_test.h` framework with assertions:
- `ASSERT_EQ(expected, actual)` - Test equality
- `ASSERT_TRUE(condition)` - Test boolean conditions
- `TEST(name, description)` - Define test cases

## Adding New Tests

1. Create new test header file (e.g., `leg_test.h`)
2. Include `unit_test.h` framework
3. Create mock objects if needed
4. Write test cases using `TEST()` macro
5. Include in `main.cpp` test runner

Example:
```cpp
#include <unit_test.h>

TEST(MyComponentTest, "Tests my component behavior") {
  MyComponent component;
  component.doSomething();
  ASSERT_EQ(42, component.getValue());
}
```

## Related

- **Integration Tests:** `tests/integration/` - Bluetooth communication tests
- **Logging Library:** `libraries/logging/` - Used by unit tests for output
