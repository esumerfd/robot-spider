#ifndef ONE_SWEEP_SEQUENCE_H
#define ONE_SWEEP_SEQUENCE_H

#include <gait_sequence.h>
#include <board.h>

/*
 * Oscillating test sequence that sweeps servos from min to max and back.
 *
 * - Alternates between moving to max and moving to min
 * - Body::atTarget() determines when to switch direction
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
    bool _movingToMax;  // Track direction: true = moving to max, false = moving to min

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

    // Toggle direction for oscillation
    void toggleDirection() { _movingToMax = !_movingToMax; }
};

#endif
