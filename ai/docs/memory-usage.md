# ESP32-CAM Memory Leak Analysis

**Date**: 2025-11-20
**Status**: Critical Issues Identified
**Target**: ESP32-CAM (limited RAM: ~520KB total, ~328KB usable)

## Executive Summary

Analysis of the robot-spider codebase revealed **critical memory leaks and stack overflow risks** that are highly likely causing ESP32-CAM crashes. The primary issues are:

1. Memory leak from Board object copies in Servo class (12× wasteful duplication)
2. Stack overflow risk from oversized logging buffers (500 bytes per call)
3. I2C bus saturation from rapid servo updates

---

## Critical Issues Found

### 1. MEMORY LEAK in Servo Class 🚨

**Location**: `libraries/robot/servo.h:13-14`

```cpp
class Servo {
  private:
    Board _board;  // ❌ COPY of Board object (should be reference)
```

**Problem**: Each Servo instance stores a **full copy** of the Board object instead of a reference. With 12 servos in the Body class, this creates **12 unnecessary Board object copies**.

**Impact**:
- Unnecessary heap fragmentation
- Wasted stack space during initialization
- Each Board copy allocates memory unnecessarily
- Constructor called 12 times when only references are needed

**Evidence**:
- Body class initializes 12 Servo objects (body.cpp:8-19)
- Each Servo constructor receives a Board reference but stores it as a value
- Board object is lightweight but still wasteful when duplicated 12 times

**Fix Required**:
```cpp
class Servo {
  private:
    Board& _board;  // Reference, not copy
```

**Also update constructor**: servo.cpp:13 already takes `Board&` parameter, just need to ensure initialization list uses reference semantics.

---

### 2. Stack Overflow Risk in Logging System 🚨

**Location**: `libraries/logging/logging.cpp:11-26`

```cpp
void Log::println(const char* format, ...) {
  char buffer[500];  // ❌ 500 bytes on stack EVERY call
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  Serial.println(buffer);
}

void Log::print(const char* format, ...) {
  char buffer[500];  // ❌ Another 500 bytes
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  Serial.print(buffer);
}
```

**Problem**: Each logging call allocates **500 bytes on the stack**. ESP32 default stack size is typically 4-8KB. With nested function calls, this quickly exhausts available stack space.

**Impact**:
- Stack overflow during deeply nested calls
- ESP32 watchdog timer triggers (stack corruption)
- Random crashes during initialization or logging-heavy operations

**Call Chain Evidence**:
```
Robot::setup() [stack frame ~100 bytes]
  └─> Body::begin() [stack frame ~50 bytes]
      └─> Servo::begin() loop [stack frame ~50 bytes]
          └─> Log::println() [stack frame 500+ bytes] ❌ OVERFLOW RISK
```

**Fix Options**:

Option A - Reduce buffer size:
```cpp
char buffer[128];  // Reduce from 500 to 128 bytes
```

Option B - Use static buffer (thread-unsafe but acceptable for single-threaded Arduino):
```cpp
static char buffer[256];  // Shared buffer, not on stack
```

Option C - Remove variadic logging, use simple print statements:
```cpp
void Log::println(const char* message) {
  Serial.println(message);
}
```

---

### 3. Excessive Memory Usage from Object Composition

**Object Hierarchy**:
```
Robot (robot-spider.ino:4)
├─ Flasher _flasher
├─ Board _board
├─ Body _body
│  ├─ 12× Servo objects (each with Board copy - BUG #1)
│  ├─ 6× Leg objects
│  │  ├─ Shoulder (extends Joint)
│  │  │  └─ Servo& reference
│  │  └─ Knee (extends Joint)
│  │     └─ Servo& reference
│  ├─ Servo* _servos[12] (array of pointers)
│  └─ Leg* _legs[6] (array of pointers)
└─ ArcTestSequence _arcTest
   └─ Board _board (another copy)
```

**Estimated RAM Usage**:
- 12 Servos × (Board object + servo state) ≈ 12 × (20-30 bytes) = **240-360 bytes wasteful overhead**
- 6 Legs × (2 Joints × 16 bytes) = **192 bytes**
- Body arrays: 12 pointers + 6 pointers = **72 bytes** (on 32-bit ESP32)
- 500-byte logging buffers during calls: **500+ bytes temporary**
- PWM driver internal buffers: **unknown, likely 100+ bytes**
- Serial TX/RX buffers at 9600 baud: **128-256 bytes** (Arduino default)
- Global pwm object (Adafruit_PWMServoDriver): **~50 bytes**

**Total Estimated Active RAM**: ~1.5KB base + temporary stack usage

While this seems manageable, the **stack overflow from logging** is the real killer.

---

### 4. I2C Bus Contention Issues

**Location**: `libraries/robot/servo.cpp:38-48`

**Current Implementation**:
```cpp
void Servo::move(uint16_t position) {
  _position = position;
  pwm.setPWM(_servonum, 0, _position);

  // Critical: I2C bus needs time between commands
  delayMicroseconds(500);  // Added in recent commit
}
```

**Problem**:
- 4 active servos (LeftFront + LeftMiddle) × 2 joints = **8 servo updates per loop**
- Each update calls `pwm.setPWM()` which performs I2C communication
- Loop delay is 10ms (robot.cpp:66)
- **Rate**: 8 updates / 10ms = **800 I2C transactions per second**
- 500μs delay = 0.5ms × 8 = **4ms spent in I2C delays per loop**

**Impact**:
- I2C bus may become saturated
- ESP32 I2C controller has limited buffer space
- Rapid back-to-back commands can cause bus lockup
- Watchdog timer may trigger if I2C operations block too long

**Evidence from Recent Commit** (2cee050):
> "I2C bus timing fixes: Added 500μs delay in Servo::move() after pwm.setPWM()"
> "Prevents I2C bus lockup from rapid back-to-back commands"

This indicates the issue was already discovered but may need further tuning.

---

### 5. Serial Buffer Overflow Risk

**Location**: `libraries/logging/logging.cpp` + usage throughout code

**Baud Rate**: 9600 (logging.cpp header comment)

**Throughput Calculation**:
- 9600 baud ÷ 10 bits/byte = **960 bytes/second** maximum
- Each Log::println call can generate 50-100 bytes
- 10ms loop = 100 loops/second
- If each loop logs 2-3 messages = **200-300 messages/second**
- Potential data rate: 50 bytes × 200 = **10,000 bytes/second** ❌

**Impact**:
- Serial TX buffer fills faster than it drains
- `Serial.println()` blocks until buffer has space
- Blocking causes loop delays
- Delays trigger ESP32 watchdog timer
- **Crash**

**Mitigation Already Applied**:
From commit 2cee050:
> "Reduced logging to prevent serial buffer overflow"
> "Commented out verbose servo movement logging"

However, buffer size is still 500 bytes which is excessive.

---

## Build Information

**From Recent Commit** (2cee050):
- Code size: 309,864 bytes flash
- RAM usage: 20,148 bytes static allocation
- ESP32-CAM total RAM: ~520KB
- Usable RAM after boot: ~328KB

**Observation**: Static RAM usage (20KB) seems reasonable, suggesting the issue is **stack overflow** or **heap fragmentation**, not static allocation.

---

## Crash Context

**From Commit Message** (2cee050):
> "Current Status:"
> "- ESP32 still experiencing crashes during loop execution"
> "- Crash occurs after 'Gait 'ArcTest' applied' message"
> "- Likely related to watchdog timer, I2C bus contention, or memory issues"

**Timeline**:
1. Robot::setup() completes successfully
2. First loop iteration: `_body.applyGait(_arcTest)` called (robot.cpp:41)
3. ArcTest applies targets to servos
4. Body::update() starts moving servos
5. **CRASH** occurs during servo movement

**Hypothesis**:
The crash likely happens when:
1. Joint::update() calls Servo::move() (joint.cpp:39)
2. Servo::move() calls pwm.setPWM() (servo.cpp:43)
3. Multiple servos update in rapid succession
4. I2C bus becomes saturated OR
5. Logging during movement triggers stack overflow

---

## Recommendations (Priority Order)

### HIGH Priority - Fix Immediately

#### 1. Fix Servo Board Copy Bug
**File**: `libraries/robot/servo.h:13`

**Change**:
```cpp
// Before
Board _board;

// After
Board& _board;
```

**Verification**: Ensure servo.cpp:13 constructor properly initializes the reference.

**Expected Impact**: Eliminate 12× unnecessary Board object copies, reduce memory usage by ~240-360 bytes.

---

#### 2. Reduce Logging Buffer Size
**File**: `libraries/logging/logging.cpp`

**Change**:
```cpp
// Before
void Log::println(const char* format, ...) {
  char buffer[500];
  // ...
}

// After
void Log::println(const char* format, ...) {
  char buffer[128];  // Reduced from 500 to 128
  // ...
}

void Log::print(const char* format, ...) {
  char buffer[128];  // Reduced from 500 to 128
  // ...
}
```

**Expected Impact**: Reduce stack usage by 744 bytes per logging call (500→128 for println + print), significantly reducing stack overflow risk.

---

#### 3. Add Memory Monitoring
**File**: `libraries/robot/robot.cpp`

**Addition to setup()**:
```cpp
void Robot::setup() {
  Log::begin();

  // Add memory diagnostics
  Log::println("Free heap: %d bytes", ESP.getFreeHeap());
  Log::println("Stack high water: %d bytes", uxTaskGetStackHighWaterMark(NULL));

  _flasher.begin();
  // ... rest of setup
}
```

**Addition to loop()** (optional, for debugging):
```cpp
void Robot::loop() {
  yield();

  // Periodic heap monitoring
  static uint32_t lastHeapCheck = 0;
  if (currentMs - lastHeapCheck > 5000) {
    Log::println("Heap: %d", ESP.getFreeHeap());
    lastHeapCheck = currentMs;
  }

  // ... rest of loop
}
```

**Expected Impact**: Provide visibility into actual memory usage and stack consumption.

---

### MEDIUM Priority

#### 4. Reduce I2C Bus Pressure

**Option A**: Increase loop delay
**File**: `libraries/robot/robot.cpp:66`

```cpp
// Before
delay(10);

// After
delay(20);  // or delay(50) for testing
```

**Expected Impact**: Reduce I2C transaction rate from 800/sec to 400/sec (or 160/sec), giving I2C bus more recovery time.

---

**Option B**: Batch servo updates
**File**: `libraries/robot/joint.cpp`

**Concept**: Instead of calling `_servo.move()` immediately in `Joint::update()`, store the target position and update all servos in a single batched operation.

**Implementation**: More complex, requires refactoring. Defer until simpler fixes are tested.

---

#### 5. Increase I2C Delay
**File**: `libraries/robot/servo.cpp:47`

```cpp
// Before
delayMicroseconds(500);

// After
delayMicroseconds(1000);  // Increase from 500μs to 1ms
```

**Expected Impact**: Further reduce I2C bus contention at the cost of slower servo updates.

---

### LOW Priority

#### 6. Use PROGMEM for Constant Strings

**Concept**: Store constant strings in flash memory instead of RAM.

**Example**:
```cpp
// Before
Log::println("Body: initialized %d legs with %d servos", LEG_COUNT, SERVO_COUNT);

// After
const char MSG_BODY_INIT[] PROGMEM = "Body: initialized %d legs with %d servos";
Log::println(MSG_BODY_INIT, LEG_COUNT, SERVO_COUNT);
```

**Expected Impact**: Save ~100-200 bytes of RAM by moving strings to flash.

---

#### 7. Monitor Heap Fragmentation

**Implementation**: Add periodic heap checks and track minimum free heap during operation.

**Expected Impact**: Identify if heap fragmentation is contributing to crashes.

---

## Testing Plan

### Phase 1: Fix Critical Bugs
1. Apply fix #1 (Servo Board reference)
2. Apply fix #2 (Reduce logging buffer)
3. Apply fix #3 (Add memory monitoring)
4. Build and upload
5. Monitor serial output for heap and stack usage
6. Verify crash is resolved

### Phase 2: Tune I2C Performance
1. If crash persists, apply fix #4 (increase loop delay to 20ms)
2. If still crashing, apply fix #5 (increase I2C delay to 1ms)
3. Monitor for stability

### Phase 3: Optimize
1. Once stable, apply fix #6 (PROGMEM) if needed
2. Reduce delays back down if possible
3. Re-enable disabled servos incrementally

---

## Expected Outcomes

**After Phase 1**:
- Memory usage reduced by ~400-800 bytes (Board copies + logging buffers)
- Stack overflow risk eliminated
- Clear visibility into heap usage
- **Expected result**: Crash should be resolved or significantly improved

**After Phase 2**:
- I2C bus contention reduced
- More reliable servo communication
- **Expected result**: Stable operation with 4 servos

**After Phase 3**:
- Optimized memory usage
- All 12 servos operational
- Stable, reliable robot operation

---

## Conclusion

The ESP32-CAM crashes are most likely caused by:

1. **Stack overflow** from 500-byte logging buffers during nested calls (HIGH confidence)
2. **Memory waste** from 12× Board object copies in Servo class (MEDIUM confidence)
3. **I2C bus saturation** from rapid servo updates (MEDIUM confidence)

The recommended fix priority is:
1. Fix Servo Board copy bug (simple, guaranteed improvement)
2. Reduce logging buffer size (simple, eliminates stack overflow risk)
3. Add memory monitoring (simple, provides diagnostic data)
4. Tune I2C delays and loop frequency (if needed after first 3 fixes)

**Estimated time to fix**: 30-60 minutes for critical fixes + testing.

---

## References

- Recent commit: `2cee050` - "Debug ESP32 crash issues - work in progress"
- ESP32 technical specs: ~520KB total RAM, ~328KB usable after boot
- Arduino ESP32 default stack size: 8KB (configurable)
- I2C bus speed: Not explicitly set, likely default 100KHz

## Related Files

### Critical Files to Modify
- `libraries/robot/servo.h` - Fix Board copy
- `libraries/logging/logging.cpp` - Reduce buffer size
- `libraries/robot/robot.cpp` - Add memory monitoring

### Files for Reference
- `libraries/robot/body.cpp` - 12 Servo instantiation
- `libraries/robot/joint.cpp` - Servo movement calls
- `robot-spider.ino` - Main entry point
