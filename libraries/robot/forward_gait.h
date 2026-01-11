#ifndef FORWARD_GAIT_H
#define FORWARD_GAIT_H

#include <gait_sequence.h>
#include <board.h>

/**
 * ForwardGait - Controls servo 0 (Left Front shoulder) for forward command.
 *
 * Oscillates servo 0 from min to max positions for testing.
 * All other servos remain unchanged.
 */
class ForwardGait : public GaitSequence {
  private:
    Board _board;
    uint16_t _speed;
    bool _movingToMax;  // Track direction: true = moving to max, false = moving to min

  public:
    ForwardGait();

    void applyTo(LeftFrontLeg& leg) override;
    void applyTo(LeftMiddleLeg& leg) override;
    void applyTo(LeftRearLeg& leg) override;
    void applyTo(RightFrontLeg& leg) override;
    void applyTo(RightMiddleLeg& leg) override;
    void applyTo(RightRearLeg& leg) override;

    const char* getName() const override { return "Forward"; }

    // Toggle direction for oscillation
    void toggleDirection() { _movingToMax = !_movingToMax; }
};

#endif
