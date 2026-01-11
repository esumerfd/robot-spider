# ADR 004: Multi-Step Gait Sequence Architecture

## Status

Proposed

## Date

2026-01-11

## Context

The current gait system uses a visitor pattern where `GaitSequence` implementations define movements by applying targets to individual legs. Each gait class (e.g., `ForwardGait`, `BackwardGait`) implements the `GaitSequence` interface with six `applyTo()` methods, one for each leg type.

**Current Architecture Strengths:**
- Clean separation of concerns via visitor pattern
- Type-safe compile-time binding
- Simple oscillating movements work well for testing individual servos
- Time-based interpolation handled by `Joint` class

**Current Limitations:**
1. **Binary Movement Only**: Existing gaits can only toggle between two positions (min/max), as seen in `ForwardGait`:
   ```cpp
   void ForwardGait::applyTo(LeftFrontLeg& leg) {
     if (_movingToMax) {
       leg.shoulder().setTarget(safeMax, _speed);
     } else {
       leg.shoulder().setTarget(safeMin, _speed);
     }
   }
   ```

2. **No Multi-Phase Sequences**: Cannot express movement patterns that require multiple coordinated steps over time (e.g., lift body → swing legs → lower body → shift forward).

3. **Single Joint Per Gait**: Current gaits typically control one joint at a time. Coordinated multi-joint movements require multiple gait objects.

4. **Ambiguous Naming**: Direction-based names like "forward" could mean rotation, translation, or crab-walk. The name doesn't describe the actual movement pattern.

5. **No Temporal Coordination**: Cannot specify "do step A, wait for completion, then do step B."

**Real-World Requirement:**

To implement forward walking, the robot must execute a complex sequence:
1. **Lift body**: LF, MR, LR knees push down (3 joints coordinated)
2. **Swing legs forward**: RF, ML, RR shoulders rotate (3 joints coordinated)
3. **Lower body**: Original 3 knees return to neutral
4. **Shift body forward**: All shoulders move to transfer weight
5. **Repeat** with opposite leg set

This cannot be expressed with current single-step, single-joint gaits.

## Investigation

### Analysis of Existing Architecture

The current system has these key components:

**GaitSequence Interface** (`libraries/robot/gait_sequence.h`):
```cpp
class GaitSequence {
  public:
    virtual void applyTo(LeftFrontLeg& leg) = 0;
    virtual void applyTo(LeftMiddleLeg& leg) = 0;
    virtual void applyTo(LeftRearLeg& leg) = 0;
    virtual void applyTo(RightFrontLeg& leg) = 0;
    virtual void applyTo(RightMiddleLeg& leg) = 0;
    virtual void applyTo(RightRearLeg& leg) = 0;
    virtual const char* getName() const = 0;
};
```

**Time-Based Movement** (`libraries/robot/joint.h`):
- Joints interpolate from current position to target over time
- `setTarget(position, speed)` initiates movement
- `update(deltaMs)` advances interpolation
- `atTarget()` reports completion

**Body Composition** (`libraries/robot/body.h`):
- Aggregates 6 legs
- `applyGait(GaitSequence&)` calls visitor pattern
- `update(deltaMs)` propagates to all joints
- `atTarget()` returns true when all joints reach targets

### Architectural Options Considered

#### Option 1: Replace GaitSequence with Sequence-Based System
**Approach**: Remove visitor pattern, introduce new base class with step execution.

**Pros**: Clean slate, optimized for sequences
**Cons**: Breaks all existing gaits, loses testing infrastructure
**Decision**: ❌ Rejected - too disruptive

#### Option 2: Extend GaitSequence with Optional Multi-Step Support
**Approach**: Add new `MultiStepGait` class that implements `GaitSequence` interface but internally manages sequence state.

**Pros**:
- Keeps existing simple gaits functional
- Backward compatible
- Can coexist indefinitely

**Cons**:
- Two different gait paradigms in codebase
- Slightly more complex architecture

**Decision**: ✅ **Selected** - preserves working code while enabling new capabilities

### Data Storage Options

#### Option 1: JSON Configuration Files
**Approach**: Define sequences in JSON, parse at runtime.

**Pros**: Easy to edit without recompiling
**Cons**: Requires JSON parser, runtime overhead, no type safety, limited by filesystem
**Decision**: ❌ Rejected - overkill for initial implementation

#### Option 2: Hardcoded C++ Arrays/Structs
**Approach**: Define sequences as `const` data structures in code.

**Pros**:
- Type-safe at compile time
- Zero parsing overhead
- Works in any environment (no filesystem required)
- Easy to debug (visible in code)

**Cons**:
- Requires recompile to change sequences
- More verbose than JSON

**Decision**: ✅ **Selected** - best for initial implementation, can add JSON later

#### Option 3: Algorithmic Generation
**Approach**: Calculate joint positions from inverse kinematics.

**Pros**: Parametric movement (adjust step length, speed, etc.)
**Cons**: Complex math, needs testing/tuning
**Decision**: 🔮 Future work - build this on top of hardcoded sequences

### Position Specification Options

#### Option 1: Absolute Positions
**Approach**: Specify exact servo PWM values for each step.

**Example**: `{servo: 0, position: 350, duration: 1000}`

**Pros**: Explicit, easy to visualize
**Cons**:
- Fragile (only works from specific starting positions)
- Cannot adapt to current state
- Difficult to tune (must recalculate all positions if one changes)

**Decision**: ❌ Rejected

#### Option 2: Relative Deltas
**Approach**: Specify position changes relative to current position.

**Example**: `{servo: 0, delta: -50, duration: 1000}` (move 50 units clockwise)

**Pros**:
- **Adaptive**: Works from any starting position
- **Composable**: Can chain sequences regardless of previous state
- **Tunable**: Adjust one delta without affecting others
- **Intuitive**: Matches how humans think about movement ("lift 50 units")

**Cons**:
- Slightly more complex calculation (current + delta)
- Accumulated error possible over many steps (can be reset)

**Decision**: ✅ **Selected** - most flexible and maintainable

#### Option 3: Mixed (Absolute + Relative)
**Approach**: Allow both absolute and relative in same sequence.

**Decision**: ❌ Rejected - adds complexity without clear benefit

### Data Structure Organization

#### Option 1: Flat Array with Leg/Joint Indices
**Approach**: Array of `JointDelta` structs with `legIndex` and `jointType` fields.

**Example**: `{legIndex: 0, jointType: 1, delta: -50, duration: 1000}`

**Cons**:
- Requires centralized loop over all deltas
- Forces leg coupling - all movements processed together
- Bypasses visitor pattern - `applyTo()` methods become stubs
- Breaks abstraction - accessing legs by index instead of type
- Incompatible with sequence generators that produce per-leg data
- Hardware coupling through index-based access

**Decision**: ❌ Rejected - couples legs together, breaks visitor pattern

#### Option 2: Per-Leg Named Fields
**Approach**: `GaitStep` has explicit named fields for each leg (`leftFront`, `leftMiddle`, etc.)

**Example**:
```cpp
struct GaitStep {
  LegMovement leftFront;
  LegMovement leftMiddle;
  // ... other legs
};
```

**Pros**:
- ✅ Preserves visitor pattern - each `applyTo()` stands alone
- ✅ No leg coupling - each method is independent
- ✅ Hardware agnostic - no index-based access
- ✅ Sequence generators naturally produce per-leg data
- ✅ Type-safe - named fields are explicit
- ✅ Dynamic dispatch preserved - `Body` orchestrates calls

**Cons**:
- Uses memory for all 6 legs even if only 1 moves (acceptable tradeoff)

**Decision**: ✅ **Selected** - preserves abstraction and enables future sequence generators

## Decision

**We will implement a `MultiStepGait` class that extends the existing `GaitSequence` interface and uses hardcoded C++ data structures with per-leg organization to define multi-step sequences with relative position deltas.**

**Key architectural principles:**
- Preserve visitor pattern - each `applyTo()` method stands alone
- Avoid leg coupling - no centralized loops over hardware indices
- Enable sequence generators - data structure matches how generators will produce movements
- Hardware agnostic - named fields instead of index-based access

### Core Data Structures

```cpp
// Represents movement for a single leg's joints
struct LegMovement {
  int16_t shoulderDelta;  // Relative position change for shoulder (0 = no movement)
  int16_t kneeDelta;      // Relative position change for knee (0 = no movement)
  uint16_t duration;      // Time to complete movement in milliseconds
};

// Represents one step in a multi-step sequence
// Each leg has explicit named field - no index coupling
struct GaitStep {
  const char* name;              // Human-readable description
  LegMovement leftFront;         // Left Front leg movement
  LegMovement leftMiddle;        // Left Middle leg movement
  LegMovement leftRear;          // Left Rear leg movement
  LegMovement rightFront;        // Right Front leg movement
  LegMovement rightMiddle;       // Right Middle leg movement
  LegMovement rightRear;         // Right Rear leg movement
  bool waitForCompletion;        // If true, wait for all joints to reach target before advancing
};

// A complete multi-step gait sequence
struct GaitSequenceData {
  const char* name;              // Sequence name (e.g., "Forward Walk")
  const GaitStep* steps;         // Array of steps
  uint8_t stepCount;             // Number of steps in sequence
  bool looping;                  // If true, repeat sequence when complete
};
```

### MultiStepGait Class

```cpp
class MultiStepGait : public GaitSequence {
  private:
    Board _board;
    const GaitSequenceData* _sequenceData;
    uint8_t _currentStepIndex;
    bool _stepInProgress;

    // Helper to apply movement to a leg's joints
    void applyLegMovement(Leg& leg, const LegMovement& movement);

    // Helper to apply a delta to a single joint
    void applyDelta(Joint& joint, int16_t delta, uint16_t duration);

  public:
    MultiStepGait(const GaitSequenceData* data);

    // GaitSequence interface - each method handles its specific leg independently
    void applyTo(LeftFrontLeg& leg) override;
    void applyTo(LeftMiddleLeg& leg) override;
    void applyTo(LeftRearLeg& leg) override;
    void applyTo(RightFrontLeg& leg) override;
    void applyTo(RightMiddleLeg& leg) override;
    void applyTo(RightRearLeg& leg) override;

    const char* getName() const override;

    // Multi-step specific control
    void advance();              // Move to next step in sequence
    bool isComplete() const;     // True if all steps executed
    void reset();                // Return to step 0
    uint8_t getCurrentStep() const;
};
```

### Example: Body Lift Sequence

This example defines the initial movement for forward walking - lifting the body using 3 knees:

```cpp
// Step 1: Lift body by extending 3 knees
// - Left Front knee: rotate clockwise (-50)
// - Right Middle knee: rotate counter-clockwise (+50)
// - Left Rear knee: rotate clockwise (-50)
const GaitStep FORWARD_WALK_STEPS[] = {
  {
    "Lift body",           // Step name
    {0, -50, 1000},        // Left Front: knee -50 over 1 second
    {0,   0,    0},        // Left Middle: no movement
    {0, -50, 1000},        // Left Rear: knee -50 over 1 second
    {0,   0,    0},        // Right Front: no movement
    {0, +50, 1000},        // Right Middle: knee +50 over 1 second
    {0,   0,    0},        // Right Rear: no movement
    true                   // Wait for all joints to complete
  },
  // Future steps would go here:
  // - Swing opposite legs forward (shoulders)
  // - Lower body (same knees return to neutral)
  // - Shift body forward (all shoulders move)
};

const GaitSequenceData FORWARD_WALK_SEQUENCE = {
  "Forward Walk",          // Sequence name
  FORWARD_WALK_STEPS,      // Array of steps
  1,                       // Currently only 1 step defined
  false                    // Don't loop (for now)
};
```

### Integration with Robot Class

```cpp
class Robot {
  private:
    // Existing simple gaits (for servo testing)
    ForwardGait _forwardGait;
    BackwardGait _backwardGait;
    LeftGait _leftGait;
    RightGait _rightGait;

    // Future: Multi-step gaits can be added alongside
    // MultiStepGait _walkForwardGait;   // Uses FORWARD_WALK_SEQUENCE

  public:
    void handleForwardCommand() {
      _body.applyGait(_forwardGait);
    }

    void handleBackwardCommand() {
      _body.applyGait(_backwardGait);
    }

    // In future, commands can be mapped to multi-step gaits:
    // void handleWalkForwardCommand() {
    //   _body.applyGait(_walkForwardGait);
    // }
};
```

## Implementation

### Phase 1: Core Data Structures (Initial Scope)

**Files to Create:**
- `libraries/robot/multi_step_gait.h` - Class definition and data structures
- `libraries/robot/multi_step_gait.cpp` - Implementation
- `libraries/robot/gait_sequences.h` - Predefined sequence definitions (e.g., FORWARD_WALK_SEQUENCE)

**Key Implementation Details:**

1. **Visitor Pattern - Per-Leg Application:**
   Each `applyTo()` method stands alone and handles only its specific leg:
   ```cpp
   void MultiStepGait::applyTo(LeftFrontLeg& leg) {
     const GaitStep& step = _sequenceData->steps[_currentStepIndex];
     applyLegMovement(leg, step.leftFront);  // Only handles LeftFront
   }

   void MultiStepGait::applyTo(LeftMiddleLeg& leg) {
     const GaitStep& step = _sequenceData->steps[_currentStepIndex];
     applyLegMovement(leg, step.leftMiddle);  // Only handles LeftMiddle
   }

   void MultiStepGait::applyTo(RightMiddleLeg& leg) {
     const GaitStep& step = _sequenceData->steps[_currentStepIndex];
     applyLegMovement(leg, step.rightMiddle);  // Only handles RightMiddle
   }

   // ... similar for other legs
   ```

   **Key Point**: Each method is independent. `Body::applyGait()` orchestrates the visitor calls. No leg coupling occurs.

2. **Leg Movement Helper:**
   Shared helper applies movement to a leg's joints:
   ```cpp
   void MultiStepGait::applyLegMovement(Leg& leg, const LegMovement& movement) {
     if (movement.shoulderDelta != 0) {
       applyDelta(leg.shoulder(), movement.shoulderDelta, movement.duration);
     }
     if (movement.kneeDelta != 0) {
       applyDelta(leg.knee(), movement.kneeDelta, movement.duration);
     }
   }
   ```

3. **Delta Application:**
   Applies relative position change to a joint:
   ```cpp
   void MultiStepGait::applyDelta(Joint& joint, int16_t delta, uint16_t duration) {
     // Calculate new target as current position + delta
     uint16_t currentPos = joint.getCurrentPos();
     int32_t newTarget = currentPos + delta;

     // Clamp to safe servo range
     const uint16_t safeMin = _board.servoMin() + 5;
     const uint16_t safeMax = _board.servoMax() - 5;
     newTarget = constrain(newTarget, safeMin, safeMax);

     // Calculate speed from distance and duration
     uint16_t distance = abs(delta);
     uint16_t speed = (distance * 1000) / duration;  // units per second

     joint.setTarget(newTarget, speed);
   }
   ```

4. **Step Advancement:**
   ```cpp
   void MultiStepGait::advance() {
     // Check if current step is complete
     if (_stepInProgress && _sequenceData->steps[_currentStepIndex].waitForCompletion) {
       // Caller should check body.atTarget() before calling advance()
       return;
     }

     _currentStepIndex++;

     // Handle looping
     if (_currentStepIndex >= _sequenceData->stepCount) {
       if (_sequenceData->looping) {
         _currentStepIndex = 0;
       } else {
         _currentStepIndex = _sequenceData->stepCount - 1;  // Stay on last step
       }
     }

     _stepInProgress = false;
   }
   ```

### Phase 2: Robot Integration (Future Work)

**Files to Modify:**
- `libraries/robot/robot.h` - Add `#include <multi_step_gait.h>`, add member `_walkForwardGait`
- `libraries/robot/robot.cpp` - Initialize multi-step gait, add new command handler (e.g., `handleWalkCommand()`)

**Testing Strategy:**
1. Implement MultiStepGait class and FORWARD_WALK_SEQUENCE definition
2. Create standalone test to verify body-lift sequence
3. When ready, add new command (e.g., "walk") that uses multi-step gait
4. Existing directional commands (forward/backward/left/right) continue using simple gaits

### Phase 3: Future Extensibility (Out of Scope)

**Planned Enhancements:**

1. **Complete Walking Sequence**: Expand `FORWARD_WALK_SEQUENCE` with additional steps
   ```cpp
   const GaitStep FORWARD_WALK_STEPS[] = {
     {"Lift body",      {0, -50, 1000}, {0, 0, 0}, {0, -50, 1000}, {0, 0, 0}, {0, +50, 1000}, {0, 0, 0}, true},
     {"Swing legs",     {0, 0, 0}, {+30, 0, 800}, {0, 0, 0}, {+30, 0, 800}, {0, 0, 0}, {+30, 0, 800}, true},
     {"Lower body",     {0, +50, 1000}, {0, 0, 0}, {0, +50, 1000}, {0, 0, 0}, {0, -50, 1000}, {0, 0, 0}, true},
     {"Shift forward",  {-20, 0, 1000}, {-20, 0, 1000}, {-20, 0, 1000}, {-20, 0, 1000}, {-20, 0, 1000}, {-20, 0, 1000}, true},
   };
   ```

2. **Sequence Generators**: Create sequences programmatically
   ```cpp
   class SequenceGenerator {
     virtual GaitSequenceData* generate() = 0;
   };

   class ForwardWalkGenerator : public SequenceGenerator {
     GaitSequenceData* generate(uint16_t stepLength, uint16_t speed) {
       // Calculates per-leg LegMovement data based on parameters
       // Returns dynamically allocated GaitSequenceData
     }
   };
   ```

   **Key Point**: Generators naturally produce per-leg data, which MultiStepGait dispatches through visitor pattern

3. **Inverse Kinematics**: Calculate joint deltas from desired body movement
   ```cpp
   GaitSequenceData* generateGaitFromIK(Vector3 bodyDelta);
   ```

4. **External Configuration**: Load sequences from JSON or filesystem
   ```cpp
   GaitSequenceData* loadSequence(const char* path);
   ```

5. **Sequence Editor**: Web interface to create/edit sequences and download as C++ code

## Consequences

### Positive

✅ **Extensible Architecture**: Can easily add new sequences without changing class structure

✅ **Type-Safe**: Compile-time validation of data structures, no runtime parsing errors

✅ **Backward Compatible**: Existing simple gaits continue to work for servo testing

✅ **Adaptive Movement**: Relative deltas work from any starting position

✅ **Clear Semantics**: Step-based structure matches natural gait description

✅ **Performance**: Zero runtime overhead for sequence definition (compile-time constants)

✅ **Debuggable**: Sequence data visible in source code, easy to understand and modify

✅ **Testable**: Can test individual steps in isolation

✅ **Visitor Pattern Preserved**: Each `applyTo()` method stands alone - no leg coupling

✅ **Hardware Agnostic**: Per-leg data structure doesn't couple to specific hardware configuration

✅ **Generator Friendly**: Sequence generators naturally produce per-leg movements

### Negative

⚠️ **Compilation Required**: Changing sequences requires recompile and re-upload

⚠️ **Flash Memory**: Each sequence consumes program memory (though minimal - ~100 bytes per step)

⚠️ **Complexity**: More complex than simple oscillating gaits

⚠️ **Learning Curve**: Developers need to understand step/delta/sequence concepts

⚠️ **Data Structure Size**: LegMovement for all 6 legs even if only 1 moves (could optimize with bitfields if memory constrained)

### Neutral

ℹ️ **Two Paradigms**: Codebase will have both simple gaits (oscillating) and multi-step gaits (sequences)

ℹ️ **Initial Scope Limited**: First implementation only covers body-lift, not complete walking

ℹ️ **Manual Tuning**: Joint deltas must be determined experimentally (future: IK can automate this)

ℹ️ **No Transition Handling**: Switching between sequences may cause jerky movement (future: blend/interpolate)

## References

### Related Code Files

- [`libraries/robot/gait_sequence.h`](../../libraries/robot/gait_sequence.h) - Base interface for all gaits
- [`libraries/robot/forward_gait.h`](../../libraries/robot/forward_gait.h) - Example simple oscillating gait
- [`libraries/robot/joint.h`](../../libraries/robot/joint.h) - Time-based movement and interpolation
- [`libraries/robot/leg.h`](../../libraries/robot/leg.h) - Leg composition (shoulder + knee)
- [`libraries/robot/body.h`](../../libraries/robot/body.h) - Body composition (6 legs)
- [`libraries/robot/robot.cpp`](../../libraries/robot/robot.cpp) - Command handling and gait selection
- [`libraries/robot/board.h`](../../libraries/robot/board.h) - Hardware constants (servo range, speed)

### External Resources

- **Hexapod Gait Patterns**: [Wikipedia - Gait](https://en.wikipedia.org/wiki/Gait)
- **Servo Control**: [Adafruit PCA9685 Guide](https://learn.adafruit.com/16-channel-pwm-servo-driver)
- **Visitor Pattern**: [Refactoring Guru - Visitor](https://refactoring.guru/design-patterns/visitor)

### Related ADRs

- [ADR 001: Servo PWM Initialization Thread Safety](001-servo-pwm-initialization-thread-safety.md)
- [ADR 002: Body and Leg Composition](002-body-and-leg-composition.md)
- [ADR 003: Command Router Visitor Pattern](003-command-router-visitor-pattern.md)

---

## Appendix: Terminology

To avoid confusion, this ADR uses the following terminology consistently:

- **Gait**: Complete movement pattern or behavior (e.g., "forward walking", "turning in place")
- **Sequence**: Ordered collection of steps that implement a gait
- **Step**: Single phase of a sequence (e.g., "lift body", "swing legs")
- **Joint Delta**: Relative position change for one joint in one step
- **Duration**: Time allocated for a joint to complete its delta movement (milliseconds)
- **Position**: Absolute servo PWM value (150-600 range for PCA9685)
- **Delta**: Relative position change (can be negative for clockwise rotation)

This clarifies the overloaded use of "gait" in the current codebase, which conflates direction (forward/backward) with movement type (oscillation).
