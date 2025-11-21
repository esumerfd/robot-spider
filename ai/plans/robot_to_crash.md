# Robot Spider Development Session - Crash Debugging

**Date**: 2025-11-20
**Status**: Work in Progress - ESP32 Crash Issues

## Session Overview

This session focused on refactoring the robot codebase to eliminate repetitive code and extend servo support from 1 to 4 servos. While the refactoring was successful, the runtime deployment revealed persistent ESP32 crash/reboot issues that remain unresolved.

## Completed Work

### 1. README Update

**File**: `README.md`

Updated the project README to be more professional and informative:

- Added clear project status badge
- Enhanced overview with hardware specs
- Reorganized Quick Start section with better formatting
- Added build commands table
- Replaced old design section with reference to detailed architecture docs
- Added current status, project structure, next steps
- Added hardware configuration details
- Added contributing guidelines

**Key Changes**:
- References `ai/docs/project-analysis.md` for detailed architecture
- Shows build stats (~332KB flash, ~21KB RAM)
- Documents all make commands
- Lists 4 connected servos (LeftFront + LeftMiddle, shoulder + knee each)

### 2. Joint Class Test Suite

**Files**:
- `tests/mock_servo.h` (new)
- `tests/joint_test.h` (new)
- `tests/tests.ino` (modified)

Created comprehensive test suite for Joint class using Option A (custom unit_test library):

**MockServo Features**:
- Implements same interface as real Servo
- Tracks move() call count and positions
- Test helper methods: `getMoveCallCount()`, `getLastMovedPosition()`, `resetCallCount()`

**Joint Test Coverage** (10 test functions):
- Initialization tests
- Movement tests (upward, downward, no overshoot)
- Edge cases (small deltaMs, high speed)
- Integration tests (servo.move() called correctly)
- Sequential targets

**Build Results**: Tests compile successfully (309,864 bytes)

### 3. Arc Test Sequence Refactoring

**File**: `libraries/robot/arc_test_sequence.cpp`, `arc_test_sequence.h`

Refactored to eliminate code duplication:

**Before**: 60+ lines of duplicated arc movement logic

**After**:
- Created `applyArcToJoint()` helper method
- Accepts `Joint&` reference (works with both Shoulder and Knee)
- Single source of truth for arc movement logic
- Extended to support LeftFrontLeg and LeftMiddleLeg (4 servos total)

**Key Feature**: Parallel directional movement based on initial position
- All joints start at `servoMiddle()` (375)
- All move toward `servoMax()` (600), then toggle to `servoMin()` (150)
- Ensures synchronized movement across all active servos

### 4. Body Class Refactoring - Data Tables

**Files**: `libraries/robot/body.h`, `body.cpp`

Eliminated repetitive code by using arrays and loops:

**Changes**:
1. Added pointer arrays for iteration:
   - `Servo* _servos[12]` - Array of pointers to all servos
   - `Leg* _legs[6]` - Array of pointers to all legs
   - Constants `SERVO_COUNT = 12`, `LEG_COUNT = 6`

2. Populated arrays in constructor (after member initialization)

3. Refactored `begin()` method:
   - **Before**: 12 individual `servo.begin()` calls
   - **After**: Loop over `_servos` array
   - Reduced from 12 lines to 3 lines

4. Refactored `update()` method:
   - **Before**: 6 individual `leg.update()` calls
   - **After**: Loop over `_legs` array
   - Reduced from 6 lines to 3 lines

5. `applyGait()` method kept individual calls:
   - Required by GaitSequence interface (compile-time type safety)
   - Added comment explaining why we can't loop

6. Added `Board& _board` reference to Body:
   - Needed for static PWM initialization

**Benefits**:
- Eliminated ~40 lines of repetitive code
- Single point of maintenance
- No performance cost (arrays initialized once)
- Same binary size (309,864 bytes)

### 5. Servo PWM Initialization Fix

**Files**: `libraries/robot/servo.h`, `servo.cpp`

Fixed critical bug where PWM driver was being initialized 12 times:

**Problem**:
- Each servo's `begin()` called `Wire.begin()`, `pwm.begin()`, `pwm.setPWMFreq(60)`
- With loop in `Body::begin()`, this happened 12 times
- Caused I2C bus corruption and ESP32 crashes

**Solution**:
1. Added static `initializePWM()` method to Servo class
2. Added static `_pwmInitialized` flag
3. `Body::begin()` calls `Servo::initializePWM()` once before loop
4. Individual `Servo::begin()` now only sets servo position

**Code**:
```cpp
// Called once in Body::begin()
void Servo::initializePWM(Board& board) {
  if (_pwmInitialized) return;

  Wire.begin(board.pwmSDA(), board.pwmSCL());
  pwm.begin();
  pwm.setPWMFreq(60);

  _pwmInitialized = true;
}

// Called per-servo in loop
void Servo::begin() {
  pwm.setPWM(_servonum, 0, _position);
}
```

### 6. Logging Cleanup

**Files**: Multiple files

Removed excessive logging that was overwhelming serial buffer:

1. **Servo::move()** (`servo.cpp:27-28`)
   - Commented out verbose per-movement logging
   - Was being called multiple times per second for 4 servos
   - At 9600 baud, was trying to send thousands of chars/sec

2. **ArcTestSequence::applyArcToJoint()** (`arc_test_sequence.cpp:23-25`)
   - Commented out per-joint logging
   - Was called 4 times on every gait application

3. **Body::applyGait()** (`body.cpp:79`)
   - Simplified to single log line: "Gait 'ArcTest' applied"

4. **Servo::initializePWM()** (`servo.cpp:30`)
   - User commented out initialization log

5. **Robot::loop()** (`robot.cpp`)
   - Removed all diagnostic logging from loop
   - Minimized output to essential messages only

## ESP32 Crash Debugging (Unresolved)

### Symptoms

The ESP32 continuously crashes and reboots with the following pattern:

```
Body: initialized 6 legs with 12 servos
Robot: setup complete
Gait 'ArcTest' applied
[CRASH - Board resets]
Body: initialized 6 legs with 12 servos
...
```

**Crash Location**: After applying gait, during or right after first `Body::update()` call

### Attempted Fixes

#### 1. Watchdog Timer Yields

Added `yield()` calls throughout code:
- `Robot::setup()` - after delay, body init, gait application
- `Robot::loop()` - at start of loop
- `Body::begin()` - between servo initializations
- `Body::update()` - between leg updates
- `Joint::update()` - after servo movements

**Result**: Still crashes

#### 2. I2C Bus Timing

Added delays to prevent I2C bus overload:
- `Servo::move()`: 500μs delay after `pwm.setPWM()`
- `Joint::update()`: yield after servo move
- **Rationale**: 4 servos × 2 joints = 8+ I2C commands per loop iteration

**Result**: Still crashes

#### 3. Moved Gait Application to Loop

Changed when initial gait is applied:
- **Before**: Applied in `Robot::setup()`
- **After**: Applied in first iteration of `Robot::loop()` using `_firstLoop` flag
- **Rationale**: Allow Arduino framework to fully initialize

**Result**: Still crashes (but later in execution)

#### 4. Delta Time Safety Cap

Added safety limit in `Robot::loop()`:
```cpp
if (deltaMs > 100) {
  deltaMs = 100; // Cap at 100ms
}
```

**Rationale**: Prevent integer overflow on first loop with large time jump

**Result**: Still crashes

#### 5. Minimal Logging

Removed all verbose logging from hot paths:
- No per-servo movement logs
- No per-leg update logs
- No per-loop diagnostic logs
- **Rationale**: Serial buffer overflow at 9600 baud

**Result**: Still crashes

### Diagnostic Logging Attempts

Progressively added logging to pinpoint crash location:

**Attempt 1**: Logged major sections of loop
```
Loop start
Flash...
Update: deltaMs=100
Check target...
```
**Result**: Crashed after "Update: deltaMs=100", before "Check target..."

**Attempt 2**: Logged before/after body.update()
```
Update: deltaMs=100
Update complete
```
**Result**: Crashed after "Update:", before "complete" - crash is IN body.update()

**Attempt 3**: Logged each leg update
```
Updating leg 0
Leg 0 done
Updating leg 1
```
**Result**: Crashed while printing "Updating" - suggests serial or memory issue

### Current Hypothesis

The crash is likely caused by one or more of:

1. **ESP32 Exception** - Actual hardware exception (need decoder output)
2. **Stack Overflow** - Deep object nesting may exceed stack
3. **I2C Hardware Issue** - One of 4 servos may have hardware problem
4. **Power Supply** - Servos drawing current causing brown-out reset
5. **Memory Corruption** - Heap exhaustion or buffer overflow
6. **Watchdog Despite Yields** - Loop still taking too long

### Code State

**Build Stats**: 309,864 bytes (9% flash), 20,148 bytes (6% RAM)

**Connected Hardware**:
- Servo 0: LeftFrontLeg shoulder
- Servo 1: LeftFrontLeg knee
- Servo 2: LeftMiddleLeg shoulder
- Servo 3: LeftMiddleLeg knee

**Movement Pattern**:
- Speed: 225 units/second (450 unit range ÷ 2 seconds)
- All joints toggle between SERVOMIN (150) and SERVOMAX (600)
- Time-based incremental movement using deltaMs

## Files Modified

### Created:
- `tests/mock_servo.h` - Mock servo for testing
- `tests/joint_test.h` - Joint class test suite

### Modified:
- `README.md` - Updated documentation
- `tests/tests.ino` - Runs Joint tests
- `libraries/robot/arc_test_sequence.h` - Added helper method
- `libraries/robot/arc_test_sequence.cpp` - Refactored, extended to 4 servos
- `libraries/robot/body.h` - Added arrays for iteration, Board reference
- `libraries/robot/body.cpp` - Refactored to use loops, added yields
- `libraries/robot/servo.h` - Added static initializePWM()
- `libraries/robot/servo.cpp` - Split initialization, added I2C delay
- `libraries/robot/joint.cpp` - Added yields, Arduino.h include
- `libraries/robot/robot.h` - Added _firstLoop flag
- `libraries/robot/robot.cpp` - Moved gait to loop, added yields, reduced logging

## Commits

### Commit 1: `96c3ad9`
"Refactor Body to use data tables and fix PWM initialization crash"
- Body refactoring with arrays
- Servo PWM initialization fix
- ArcTestSequence refactoring
- Logging improvements
- Joint test suite

### Commit 2: `2cee050`
"Debug ESP32 crash issues - work in progress"
- Added yields throughout
- I2C bus timing fixes
- Moved gait to loop
- Reduced logging
- Added deltaMs safety cap

## Next Steps for Tomorrow

### Immediate Debugging

1. **Get ESP32 Exception Details**:
   - Use Arduino IDE ESP32 Exception Decoder
   - Analyze actual crash reason from serial output
   - Look for stack trace or exception code

2. **Test with Single Servo**:
   - Comment out all but LeftFrontLeg shoulder in arc_test_sequence
   - Verify single servo doesn't crash
   - Add servos back one at a time

3. **Increase I2C Delay**:
   - Try 1-2ms delay instead of 500μs
   - Test if slower I2C communication helps

4. **Check Power Supply**:
   - Monitor voltage during movement
   - Check for brown-out resets
   - May need external power for servos

5. **Test Without Movement**:
   - Apply gait but don't call body.update()
   - See if crash happens without actual servo movement

### Code Improvements (After Crash Fixed)

1. Re-enable informative logging (but minimal)
2. Add configuration for connected servos
3. Create proper error handling
4. Add servo position validation
5. Document I2C timing requirements

## Architecture Notes

### Successful Patterns

1. **Data Table Pattern**: Eliminated repetition in Body class
2. **Static Initialization**: Prevented multiple PWM init
3. **Time-Based Movement**: Clean separation using deltaMs
4. **Stateless Sequences**: Reusable movement patterns
5. **Type-Safe Dispatch**: GaitSequence with named leg methods

### Issues to Address

1. **Crash Resolution**: Critical blocker for all servo testing
2. **Power Management**: May need separate servo power
3. **I2C Reliability**: Need robust error handling
4. **Memory Usage**: Monitor heap/stack usage more carefully
5. **Timing Constraints**: Balance between smooth movement and system stability

## Hardware Configuration

- **Board**: ESP32-CAM
- **PWM Driver**: Adafruit PWM Servo Driver (I2C 0x40, 60Hz)
- **I2C Pins**: SDA=15, SCL=14
- **LED Pin**: GPIO 33
- **Serial**: 9600 baud, 8N1
- **Servo Range**: 150-600 PWM (middle: 375)
- **Active Servos**: 0, 1, 2, 3 (LeftFront + LeftMiddle legs)

---

**End of Session Summary**
