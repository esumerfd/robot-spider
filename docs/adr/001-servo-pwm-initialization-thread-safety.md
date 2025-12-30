# ADR 001: Servo PWM Initialization Thread Safety

## Status
Accepted

## Date
2025-12-30

## Context

The `Servo` class uses a static `_pwmInitialized` flag to ensure the PWM driver is initialized only once across all servo instances. The current implementation in `servo.cpp:20-37` uses a simple boolean check without explicit synchronization:

```cpp
void Servo::initializePWM(Board& board) {
  if (_pwmInitialized) {
    return; // Already initialized
  }

  Wire.begin(board.pwmSDA(), board.pwmSCL());
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);
  delay(10);

  _pwmInitialized = true;
  Log::println("Servo: PWM driver initialized");
}
```

This pattern could be vulnerable to race conditions in multi-threaded environments where multiple threads might call `initializePWM()` simultaneously, potentially causing:
- Double initialization of the I2C bus
- Concurrent calls to PWM driver initialization
- Undefined behavior in the Adafruit_PWMServoDriver library

## Investigation Findings

### Current Threading Model
Analysis of the codebase reveals:

1. **Single-threaded Arduino pattern**: The application follows the standard Arduino model with `setup()` and `loop()` functions (robot-spider.ino:6-12)

2. **No FreeRTOS task creation**: Search for `xTaskCreate`, `TaskHandle`, and FreeRTOS primitives found zero matches

3. **No std::thread usage**: No evidence of C++ threading constructs

4. **No interrupt-based initialization**: Search for `IRAM_ATTR` and `attachInterrupt` found zero matches - no ISRs that could trigger initialization

5. **Single initialization point**: PWM initialization occurs exactly once:
   - `Robot::setup()` → `Body::begin()` → `Servo::initializePWM()` (body.cpp:52-53)
   - Called during Arduino's `setup()` phase before `loop()` starts

### ESP32 Architecture Considerations

While the ESP32 is a dual-core processor and Arduino-ESP32 framework runs on FreeRTOS, the application code:
- Does not create additional tasks
- Does not use core affinity APIs
- Relies on the Arduino abstraction's single-threaded guarantees
- All servo operations occur in the main loop task

## Decision

**Do NOT add thread-safety mechanisms** to the `_pwmInitialized` flag or `initializePWM()` method.

### Rationale

1. **YAGNI Principle**: Adding synchronization for a problem that doesn't exist violates "You Aren't Gonna Need It"
   - No current threading that could cause races
   - Adding mutexes/atomics increases complexity without benefit

2. **Performance**: Even minimal synchronization (atomic operations, mutexes) adds overhead
   - Called once during startup, but avoiding unnecessary complexity is good practice
   - Atomic operations require special CPU instructions
   - Mutex operations involve FreeRTOS scheduler calls

3. **Code Clarity**: The current simple pattern is clear and maintainable
   - Developers can immediately understand the check-and-initialize pattern
   - No need to understand synchronization primitives

4. **Adequate Safety**: The single-threaded model provides sufficient guarantees
   - Initialization happens sequentially during `setup()`
   - No concurrent access paths exist

5. **Explicit Documentation**: This ADR and code comments serve as better protection than premature optimization
   - Future developers will understand the threading assumptions
   - Clear guidance on when synchronization becomes necessary

## Consequences

### Positive
- Minimal code complexity
- No performance overhead
- Clear, readable initialization code
- Explicit documentation of threading model

### Negative
- Code will require modification if threading is added later
- Developers must understand the single-threaded assumption

### Neutral
- Current implementation remains unchanged
- Technical debt is explicitly documented rather than hidden in defensive code

## When Thread Safety Becomes Necessary

Add synchronization if ANY of these conditions arise:

1. **FreeRTOS Task Creation**: Using `xTaskCreate()` or `xTaskCreatePinnedToCore()` for servo-related operations

2. **Multi-Core Execution**: Explicitly pinning servo operations to different cores

3. **Interrupt-Driven Initialization**: If ISRs or timer callbacks could trigger `initializePWM()`

4. **Concurrent Servo Usage**: Multiple execution contexts (tasks, cores, ISRs) accessing servo initialization

### Recommended Approach When Adding Thread Safety

If threading is added, use `std::call_once` for clean, guaranteed single initialization:

```cpp
#include <mutex>

static std::once_flag _initFlag;

void Servo::initializePWM(Board& board) {
  std::call_once(_initFlag, [&board]() {
    Wire.begin(board.pwmSDA(), board.pwmSCL());
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(50);
    delay(10);
    Log::println("Servo: PWM driver initialized");
  });
}
```

Benefits:
- Guaranteed single execution even with concurrent calls
- No manual flag management
- Standard C++11 construct
- Minimal overhead after first call

## Implementation

1. Add comment to `servo.h:18` documenting the threading assumption:
   ```cpp
   // PWM initialization flag - safe for single-threaded Arduino model
   // WARNING: Add synchronization if using FreeRTOS tasks or multi-core execution
   static bool _pwmInitialized;
   ```

2. Add comment to `servo.cpp:20` explaining the decision:
   ```cpp
   void Servo::initializePWM(Board& board) {
     // Single-threaded check-and-initialize pattern
     // See ADR-001 for threading model assumptions and when to add synchronization
     if (_pwmInitialized) {
       return; // Already initialized
     }
     // ... initialization code ...
   }
   ```

3. Update task documentation with findings and decision

## References
- Task 2: Implement thread-safe initialization for Servo PWM driver
- Source files: `libraries/robot/servo.h`, `libraries/robot/servo.cpp`
- Initialization call chain: `robot-spider.ino:setup()` → `robot.cpp:Robot::setup()` → `body.cpp:Body::begin()`
- ESP32-Arduino threading model: https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/threading.html
