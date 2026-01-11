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

## Decision

**We will implement a `MultiStepGait` class that extends the existing `GaitSequence` interface and uses hardcoded C++ data structures to define multi-step sequences with relative position deltas.**

### Core Data Structures

```cpp
// Represents a single joint movement within a step
struct JointDelta {
  uint8_t legIndex;    // 0-5: LF, LM, LR, RF, RM, RR
  uint8_t jointType;   // 0=shoulder, 1=knee
  int16_t delta;       // Relative position change (negative=clockwise, positive=counter-clockwise)
  uint16_t duration;   // Time to complete movement in milliseconds
};

// Represents one step in a multi-step sequence
struct GaitStep {
  const char* name;              // Human-readable description
  const JointDelta* deltas;      // Array of joint movements in this step
  uint8_t deltaCount;            // Number of elements in deltas array
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

    // Apply current step's deltas to all joints
    void applyCurrentStep(Body& body);

    // Get leg by index (0-5)
    Leg* getLeg(Body& body, uint8_t legIndex);

    // Get joint from leg by type
    Joint* getJoint(Leg* leg, uint8_t jointType);

  public:
    MultiStepGait(const GaitSequenceData* data);

    // GaitSequence interface - applies current step
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
// - Left Front knee (leg 0, joint 1): rotate clockwise (-50)
// - Middle Right knee (leg 4, joint 1): rotate counter-clockwise (+50)
// - Left Rear knee (leg 2, joint 1): rotate clockwise (-50)
const JointDelta BODY_LIFT_DELTAS[] = {
  {0, 1, -50, 1000},  // LF knee: -50 units over 1 second
  {4, 1, +50, 1000},  // MR knee: +50 units over 1 second
  {2, 1, -50, 1000},  // LR knee: -50 units over 1 second
};

const GaitStep FORWARD_WALK_STEPS[] = {
  {
    "Lift body",           // Step name
    BODY_LIFT_DELTAS,      // Joint movements
    3,                     // Number of joints involved
    true                   // Wait for all joints to complete
  },
  // Future steps would go here:
  // - Swing opposite legs forward
  // - Lower body
  // - Shift body forward
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

1. **Relative Delta Application:**
   ```cpp
   void MultiStepGait::applyCurrentStep(Body& body) {
     const GaitStep& step = _sequenceData->steps[_currentStepIndex];

     for (uint8_t i = 0; i < step.deltaCount; i++) {
       const JointDelta& delta = step.deltas[i];

       Leg* leg = getLeg(body, delta.legIndex);
       Joint* joint = getJoint(leg, delta.jointType);

       // Calculate new target as current position + delta
       uint16_t currentPos = joint->getCurrentPos();
       int32_t newTarget = currentPos + delta.delta;

       // Clamp to safe servo range
       const uint16_t safeMin = _board.servoMin() + 5;
       const uint16_t safeMax = _board.servoMax() - 5;
       newTarget = constrain(newTarget, safeMin, safeMax);

       // Calculate speed from distance and duration
       uint16_t distance = abs(delta.delta);
       uint16_t speed = (distance * 1000) / delta.duration;  // units per second

       joint->setTarget(newTarget, speed);
     }

     _stepInProgress = true;
   }
   ```

2. **Step Advancement:**
   ```cpp
   void MultiStepGait::advance() {
     // Check if current step is complete
     if (_stepInProgress && _sequenceData->steps[_currentStepIndex].waitForCompletion) {
       // Caller should check body.atTarget() before calling advance()
       // This is just a safety check
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

3. **Visitor Pattern Integration:**
   ```cpp
   void MultiStepGait::applyTo(LeftFrontLeg& leg) {
     // The actual work is done in applyCurrentStep() which operates on Body
     // These methods exist to satisfy the GaitSequence interface
     // They're effectively no-ops since we need access to all legs simultaneously
   }
   ```

   **Note**: The visitor pattern forces per-leg method calls, but multi-step sequences need to coordinate across multiple legs. The solution is to do the actual work in a separate `applyCurrentStep(Body&)` method that `Body::applyGait()` can call, or to store references during the first `applyTo()` call and execute on the last one.

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
     {"Lift body", BODY_LIFT_DELTAS, 3, true},
     {"Swing legs forward", LEG_SWING_DELTAS, 3, true},
     {"Lower body", BODY_LOWER_DELTAS, 3, true},
     {"Shift body forward", BODY_SHIFT_DELTAS, 6, true},
   };
   ```

2. **Parametric Sequences**: Generate sequences programmatically
   ```cpp
   GaitSequenceData* createForwardWalk(uint16_t stepLength, uint16_t speed);
   ```

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

### Negative

⚠️ **Compilation Required**: Changing sequences requires recompile and re-upload

⚠️ **Flash Memory**: Each sequence consumes program memory (though minimal - ~100 bytes per step)

⚠️ **Complexity**: More complex than simple oscillating gaits

⚠️ **Learning Curve**: Developers need to understand step/delta/sequence concepts

⚠️ **Visitor Pattern Mismatch**: GaitSequence interface designed for single-step, not multi-step (workaround required)

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
