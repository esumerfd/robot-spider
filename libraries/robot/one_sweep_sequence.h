#ifndef ONE_SWEEP_SEQUENCE_H
#define ONE_SWEEP_SEQUENCE_H

#include <gait_sequence.h>
#include <board.h>

/*
 * Simple test sequence that sweeps servos from min to max once.
 *
 * - Starts at current position
 * - Sets target to max position
 * - Body::atTarget() determines when movement is complete
 *
 * Speed calculation:
 * - Range: SERVOMAX (600) - SERVOMIN (150) = 450 units
 * - Duration: 2000ms = 2 seconds
 * - Speed: 450 units / 2 seconds = 225 units/second
 */
class OneSweepSequence : public GaitSequence {
  private:
    Board _board;
    uint16_t _speed;

    // Helper method to apply sweep movement to a joint
    void applySweepToJoint(Joint& joint);

  public:
    OneSweepSequence();

    void applyTo(LeftFrontLeg& leg) override;
    void applyTo(LeftMiddleLeg& leg) override;
    void applyTo(LeftRearLeg& leg) override;
    void applyTo(RightFrontLeg& leg) override;
    void applyTo(RightMiddleLeg& leg) override;
    void applyTo(RightRearLeg& leg) override;

    const char* getName() const override { return "OneSweep"; }
};

#endif
