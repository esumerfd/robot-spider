#ifndef GAIT_SEQUENCES_H
#define GAIT_SEQUENCES_H

#include "multi_step_gait.h"

// Stationary Sequence
// Robot at rest - no movement on any joints
const GaitStep STATIONARY_STEPS[] = {
  {
    "Stationary",
    {0, 0, 0},  // Left Front: no movement
    {0, 0, 0},  // Left Middle: no movement
    {0, 0, 0},  // Left Rear: no movement
    {0, 0, 0},  // Right Front: no movement
    {0, 0, 0},  // Right Middle: no movement
    {0, 0, 0},  // Right Rear: no movement
    true
  }
};

const GaitSequenceData STATIONARY_SEQUENCE = {
  "Stationary",
  STATIONARY_STEPS,
  1,
  false  // Don't loop
};

// Forward Walk Sequence
// Initial step: Lift body using 3 knees (Left Front, Right Middle, Left Rear)

// Step 1: Lift body by extending 3 knees
// - Left Front knee: rotate clockwise (-23 degrees)
// - Right Middle knee: rotate counter-clockwise (+23 degrees)
// - Left Rear knee: rotate clockwise (-23 degrees)
// - Duration 0 = constant speed (180°/s), so 23° takes ~128ms
const GaitStep FORWARD_WALK_STEPS[] = {
  {
    "Lift body",           // Step name
    {0, -23, 0},           // Left Front: knee -23° at constant speed
    {0,   0, 0},           // Left Middle: no movement
    {0, -23, 0},           // Left Rear: knee -23° at constant speed
    {0,   0, 0},           // Right Front: no movement
    {0, +23, 0},           // Right Middle: knee +23° at constant speed
    {0,   0, 0},           // Right Rear: no movement
    true                   // Wait for all joints to complete
  },
  // Future steps will go here:
  // - Swing opposite legs forward (shoulders)
  // - Lower body (same knees return to neutral)
  // - Shift body forward (all shoulders move)
};

const GaitSequenceData FORWARD_WALK_SEQUENCE = {
  "Forward Walk",          // Sequence name
  FORWARD_WALK_STEPS,      // Array of steps
  1,                       // Currently only 1 step defined
  false                    // Don't loop
};

// Backward Sequence (servo testing)
// Oscillates Left Front knee (servo 1) at constant speed
// Duration 0 = constant 180°/s, so 91° takes ~506ms
const GaitStep BACKWARD_STEPS[] = {
  {
    "Backward move",
    {0, 91, 0},         // Left Front knee: +91° at constant speed
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    true
  },
  {
    "Backward return",
    {0, -91, 0},        // Left Front knee: -91° at constant speed
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    true
  }
};

const GaitSequenceData BACKWARD_SEQUENCE = {
  "Backward",
  BACKWARD_STEPS,
  2,
  false                   // Don't loop
};

// Left Sequence (servo testing)
// Oscillates Left Middle shoulder (servo 2) at constant speed
const GaitStep LEFT_STEPS[] = {
  {
    "Left move",
    {0, 0, 0},
    {91, 0, 0},         // Left Middle shoulder: +91° at constant speed
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    true
  },
  {
    "Left return",
    {0, 0, 0},
    {-91, 0, 0},        // Left Middle shoulder: -91° at constant speed
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    true
  }
};

const GaitSequenceData LEFT_SEQUENCE = {
  "Left",
  LEFT_STEPS,
  2,
  false                   // Don't loop
};

// Right Sequence (servo testing)
// Oscillates Left Middle knee (servo 3) at constant speed
const GaitStep RIGHT_STEPS[] = {
  {
    "Right move",
    {0, 0, 0},
    {0, 91, 0},         // Left Middle knee: +91° at constant speed
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    true
  },
  {
    "Right return",
    {0, 0, 0},
    {0, -91, 0},        // Left Middle knee: -91° at constant speed
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    true
  }
};

const GaitSequenceData RIGHT_SEQUENCE = {
  "Right",
  RIGHT_STEPS,
  2,
  false                   // Don't loop
};

#endif
