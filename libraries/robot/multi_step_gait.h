#ifndef MULTI_STEP_GAIT_H
#define MULTI_STEP_GAIT_H

#include "gait_sequence.h"
#include "board.h"
#include "leg.h"
#include <profiler.h>

// Represents movement for a single leg's joints
struct LegMovement {
  int8_t shoulderDelta;  // Relative angle change for shoulder in degrees (0 = no movement)
  int8_t kneeDelta;      // Relative angle change for knee in degrees (0 = no movement)
  uint16_t duration;     // Time to complete movement in milliseconds
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

class MultiStepGait : public GaitSequence {
  private:
    Board _board;
    const GaitSequenceData* _sequenceData;
    uint8_t _currentStepIndex;
    bool _stepInProgress;

    // Call rate profiling
    CallRateProfiler _applyProfiler;

    // Helper to apply movement to a leg's joints
    void applyLegMovement(Leg& leg, const LegMovement& movement);

    // Helper to apply a delta to a single joint
    void applyDelta(Joint& joint, int8_t delta, uint16_t duration);

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

    // Profiling control
    void updateProfiler(uint32_t currentMs);
    void enableProfiling(bool enabled);
    CallRateProfiler& getProfiler();
};

#endif
