#ifndef I_GAIT_TARGET_H
#define I_GAIT_TARGET_H

#include <stdint.h>

// Forward declaration
class GaitSequence;

/**
 * Interface for anything that can receive and execute gait commands.
 *
 * This abstraction allows gaits to be applied to:
 * - Real hardware (Body with servos)
 * - Test implementations (MockBody for testing logic)
 */
class IGaitTarget {
  public:
    virtual ~IGaitTarget() = default;

    // Apply a gait sequence (sets joint targets)
    virtual void applyGait(GaitSequence& gait) = 0;

    // Update all joints based on elapsed time
    virtual void update(uint32_t deltaMs) = 0;

    // Check if all joints have reached their targets
    virtual bool atTarget() const = 0;

    // Reset all joints to middle position (90 degrees)
    virtual void resetToMiddle() = 0;

    // Get a description of current state (for logging/debugging)
    virtual void logState() const = 0;
};

#endif
