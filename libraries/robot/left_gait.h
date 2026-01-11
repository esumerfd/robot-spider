#ifndef LEFT_GAIT_H
#define LEFT_GAIT_H

#include <gait_sequence.h>
#include <board.h>

/**
 * LeftGait - Controls servo 2 (Left Middle shoulder) for left command.
 *
 * Oscillates servo 2 from min to max positions for testing.
 * All other servos remain unchanged.
 */
class LeftGait : public GaitSequence {
  private:
    Board _board;
    uint16_t _speed;
    bool _movingToMax;  // Track direction: true = moving to max, false = moving to min

  public:
    LeftGait();

    void applyTo(LeftFrontLeg& leg) override;
    void applyTo(LeftMiddleLeg& leg) override;
    void applyTo(LeftRearLeg& leg) override;
    void applyTo(RightFrontLeg& leg) override;
    void applyTo(RightMiddleLeg& leg) override;
    void applyTo(RightRearLeg& leg) override;

    const char* getName() const override { return "Left"; }

    // Toggle direction for oscillation
    void toggleDirection() { _movingToMax = !_movingToMax; }
};

#endif
