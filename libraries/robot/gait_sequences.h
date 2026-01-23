#ifndef GAIT_SEQUENCES_H
#define GAIT_SEQUENCES_H

#include "multi_step_gait.h"

// Helper macro to calculate array length at compile time
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

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
  ARRAY_LENGTH(STATIONARY_STEPS),
  false  // Don't loop
};

// Forward Walk Sequence
// Step 1: Lift body by extending 3 knees (tripod A)
// Step 2: Swing opposite tripod (B) shoulders forward
const GaitStep FORWARD_WALK_STEPS[] = {
  {
    "Lift body",           // Step name
    {0, -23, 0},           // Left Front: knee -23°
    {0,   0, 0},           // Left Middle: no movement
    {0, -23, 0},           // Left Rear: knee -23°
    {0,   0, 0},           // Right Front: no movement
    {0, +23, 0},           // Right Middle: knee +23°
    {0,   0, 0},           // Right Rear: no movement
    true                   // Wait for all joints to complete
  },
  {
    "Swing shoulders forward",
    {0,    0, 0},          // Left Front: no movement
    {+10,  0, 0},          // Left Middle: shoulder +10°
    {0,    0, 0},          // Left Rear: no movement
    {-10,  0, 0},          // Right Front: shoulder -10°
    {0,    0, 0},          // Right Middle: no movement
    {-10,  0, 0},          // Right Rear: shoulder -10°
    true                   // Wait for all joints to complete
  },
  // Future steps:
  // - Lower body (knees return to neutral)
  // - Shift body forward (all shoulders move)
};

const GaitSequenceData FORWARD_WALK_SEQUENCE = {
  "Forward Walk",
  FORWARD_WALK_STEPS,
  ARRAY_LENGTH(FORWARD_WALK_STEPS),
  false  // Don't loop
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
  ARRAY_LENGTH(BACKWARD_STEPS),
  false  // Don't loop
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
  ARRAY_LENGTH(LEFT_STEPS),
  false  // Don't loop
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
  ARRAY_LENGTH(RIGHT_STEPS),
  false  // Don't loop
};

#endif
