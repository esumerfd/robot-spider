# PWM-to-Angle Servo Interface Conversion Plan

## Overview

Convert the robot's servo interface from PWM values (150-545) to angles (0-180°) with per-servo calibration offsets throughout the entire codebase.

## Conversion Mathematics

**Linear Mapping:**
- PWM 150 → 0°
- PWM 347 → 90° (middle)
- PWM 545 → 180°
- Formula: `pwm = 150 + (angle × 395 / 180)`

**With Calibration:**
- Each servo has an offset (e.g., servo 0: -5°, servo 1: +3°)
- Applied during angle→PWM conversion: `calibratedAngle = angle + offset[servoNum]`

**Safety Range:**
- Current: PWM 155-540 (±5 unit margin)
- New: 2°-178° (cleaner numbers, equivalent margin)

**Gait Delta Conversion:**
- PWM delta ±50 → ±23° (formula: `angle ≈ pwm × 0.456`)
- PWM delta ±200 → ±91°

## Implementation Phases

### Phase 1: Board Layer (Foundation)
**Files:** `board.h`, `board.cpp`

**Add:**
- Calibration offset array: `static const int8_t SERVO_CALIBRATION_OFFSETS[12]` (all zeros initially)
- Conversion methods:
  - `uint16_t angleToPWM(uint8_t servoNum, float angle)` - Apply calibration, convert to PWM, safety clamp
  - `float pwmToAngle(uint8_t servoNum, uint16_t pwm)` - Inverse conversion, remove calibration
  - `int8_t getServoCalibrationOffset(uint8_t servoNum)`

**Change:**
- All getter return types: `uint16_t` → `float`
- Return values: `servoMin()` = 0.0°, `servoMax()` = 180.0°, `servoMiddle()` = 90.0°, `servoSpeed()` = 90.0°/sec

### Phase 2: Servo Layer
**Files:** `servo.h`, `servo.cpp`

**Change:**
- `_position`: `uint16_t` → `float _positionAngle`
- `move()` parameter: `uint16_t position` → `float angle`
- `getPosition()` return: `uint16_t` → `float`
- Initial position: 347 PWM → 90.0°
- Add conversion call: `uint16_t pwm = _board.angleToPWM(_servonum, angle)` before hardware write

### Phase 3: Joint Layer
**Files:** `joint.h`, `joint.cpp`

**Change:**
- All position/speed types: `uint16_t` → `float`
- Constructor parameter: initial position in degrees
- `atTarget()` comparison: `==` → `abs(delta) < 0.5f` (float tolerance)
- Speed calculations: use float math (`/ 1000.0f`)
- Log format: `%d` → `%.1f°`

### Phase 4: Leg Layer
**Files:** `leg.h`, `leg.cpp`

**Change:**
- Constructor parameters: `uint16_t` → `float` for initial positions
- Pass-through only, minimal changes

### Phase 5: Body Layer
**Files:** `body.cpp`

**Change:**
- Leg initialization: use `90.0f` instead of `board.servoMiddle()`
- `resetToMiddle()`: use `_board.servoMiddle()` (returns 90.0f)

### Phase 6: Gait Sequences
**Files:** `multi_step_gait.h`, `multi_step_gait.cpp`, `gait_sequences.h`

**Change:**
- `LegMovement` delta types: `int16_t` → `int8_t` (±127° sufficient)
- `applyDelta()`: use float math, angle-based safety clamping (2°-178°)
- **Convert all gait deltas:**
  - `FORWARD_WALK_STEPS`: ±50 PWM → ±23°
  - `BACKWARD_STEPS`: ±200 PWM → ±91°
  - `LEFT_STEPS`: ±200 PWM → ±91°
  - `RIGHT_STEPS`: ±200 PWM → ±91°

### Phase 7: Test Sequences
**Files:** `one_sweep_sequence.cpp`

**Change:**
- Safety range: use `_board.servoMinAngle() + 2.0f` and `_board.servoMaxAngle() - 2.0f`
- Direct angle setting (2° and 178°)

## Critical Files

1. **board.cpp** - Core conversion logic and calibration storage
2. **joint.cpp** - Movement interpolation with float math
3. **gait_sequences.h** - All gait deltas must be converted
4. **multi_step_gait.cpp** - Delta application logic
5. **servo.cpp** - Hardware interface conversion

## Gait Delta Conversion Reference

| PWM Delta | Angle Delta |
|-----------|-------------|
| ±50       | ±23°        |
| ±100      | ±46°        |
| ±150      | ±68°        |
| ±200      | ±91°        |

Formula: `angleDelta ≈ round(pwmDelta × 0.456)`

## Calibration System

**Storage:** Static array in `board.cpp`
```cpp
const int8_t Board::SERVO_CALIBRATION_OFFSETS[12] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // All zeros initially
};
```

**Calibration Process:**
1. Set all offsets to 0
2. Run `reset` command (all servos to 90°)
3. Observe misaligned servos
4. Update offsets (e.g., servo 0: -5° if 5° too far clockwise)
5. Recompile and test

**Offset Range:** -20° to +20° recommended

## Testing Strategy

1. **Unit Tests:** Conversion functions (0°→150 PWM, 90°→347 PWM, 180°→545 PWM)
2. **Integration Tests:** Single servo movement, joint interpolation
3. **Gait Tests:** Stationary, sweep, directional movements, forward walk
4. **Calibration:** Iterative adjustment until all servos align at 90°
5. **Regression:** Verify all Bluetooth commands still work

## Verification

**Expected Behavior:**
- Reset command: All servos move to 90° (visually centered)
- Forward walk: Same physical movement as before (±23° ≈ ±50 PWM)
- Sweep: Full range 2°-178° (equivalent to previous 155-540 PWM)
- Speed: 90°/sec feels identical to 197.5 PWM units/sec (both are half-range per second)

**Log Output Changes:**
- Before: `Joint: cur=342 tgt=297 speed=50 deltaMs=100`
- After: `Joint: cur=155.5° tgt=135.0° speed=22.8°/s deltaMs=100`

## Key Benefits

1. **Intuitive:** Degrees are easier to understand than PWM values
2. **Calibrated:** Per-servo offsets correct mechanical variations
3. **Maintainable:** Gait sequences more readable (±23° vs ±50 PWM)
4. **Hardware-agnostic:** Angles portable across different servo controllers

## Potential Risks

**Medium Risk:**
- Float precision accumulation (mitigated by periodic reset and clamping)
- Gait delta conversions (requires manual verification)

**Low Risk:**
- Type changes (straightforward)
- Math conversions (simple linear mapping)

**Mitigation:** Implement in phases with testing at each layer

## Estimated Effort

- Implementation: 8-10 hours
- Testing & Calibration: 2-4 hours
- Documentation: 1-2 hours
- **Total: 11-16 hours**
