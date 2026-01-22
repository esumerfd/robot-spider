#ifndef GAIT_SEQUENCE_H
#define GAIT_SEQUENCE_H

#include <left_front_leg.h>
#include <left_middle_leg.h>
#include <left_rear_leg.h>
#include <right_front_leg.h>
#include <right_middle_leg.h>
#include <right_rear_leg.h>

/*
 * Interface for gait sequences.
 *
 * Sequences are stateless descriptions of movement that can be
 * applied to legs. Each leg type gets its own apply method,
 * allowing leg-specific positioning and timing.
 *
 * Sequences can be reapplied to continue a movement pattern.
 */
class GaitSequence {
  public:
    virtual ~GaitSequence() = default;

    // Apply sequence to each leg type
    virtual void applyTo(LeftFrontLeg& leg) = 0;
    virtual void applyTo(LeftMiddleLeg& leg) = 0;
    virtual void applyTo(LeftRearLeg& leg) = 0;
    virtual void applyTo(RightFrontLeg& leg) = 0;
    virtual void applyTo(RightMiddleLeg& leg) = 0;
    virtual void applyTo(RightRearLeg& leg) = 0;

    // Get sequence name for debugging/logging
    virtual const char* getName() const = 0;

    // Get current step name (for multi-step gaits)
    // Returns nullptr for single-step gaits
    virtual const char* getStepName() const { return nullptr; }

    // Get current step index (for multi-step gaits)
    // Returns 0 for single-step gaits
    virtual uint8_t getStepIndex() const { return 0; }
};

#endif
