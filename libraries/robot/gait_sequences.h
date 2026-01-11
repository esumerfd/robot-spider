#ifndef GAIT_SEQUENCES_H
#define GAIT_SEQUENCES_H

#include "multi_step_gait.h"

// Forward Walk Sequence
// Initial step: Lift body using 3 knees (Left Front, Right Middle, Left Rear)

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
  // Future steps will go here:
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

#endif
