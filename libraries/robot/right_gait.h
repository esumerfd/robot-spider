#ifndef RIGHT_GAIT_H
#define RIGHT_GAIT_H

#include <gait_sequence.h>
#include <board.h>

/**
 * RightGait - Controls servo 3 (Left Middle knee) for right command.
 *
 * Oscillates servo 3 from min to max positions for testing.
 * All other servos remain unchanged.
 */
class RightGait : public GaitSequence {
  private:
    Board _board;
    uint16_t _speed;
    bool _movingToMax;  // Track direction: true = moving to max, false = moving to min

  public:
    RightGait();

    void applyTo(LeftFrontLeg& leg) override;
    void applyTo(LeftMiddleLeg& leg) override;
    void applyTo(LeftRearLeg& leg) override;
    void applyTo(RightFrontLeg& leg) override;
    void applyTo(RightMiddleLeg& leg) override;
    void applyTo(RightRearLeg& leg) override;

    const char* getName() const override { return "Right"; }

    // Toggle direction for oscillation
    void toggleDirection() { _movingToMax = !_movingToMax; }
};

#endif
