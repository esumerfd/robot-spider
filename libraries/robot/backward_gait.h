#ifndef BACKWARD_GAIT_H
#define BACKWARD_GAIT_H

#include <gait_sequence.h>
#include <board.h>

/**
 * BackwardGait - Controls servo 1 (Left Front knee) for backward command.
 *
 * Oscillates servo 1 from min to max positions for testing.
 * All other servos remain unchanged.
 */
class BackwardGait : public GaitSequence {
  private:
    Board _board;
    uint16_t _speed;
    bool _movingToMax;  // Track direction: true = moving to max, false = moving to min

  public:
    BackwardGait();

    void applyTo(LeftFrontLeg& leg) override;
    void applyTo(LeftMiddleLeg& leg) override;
    void applyTo(LeftRearLeg& leg) override;
    void applyTo(RightFrontLeg& leg) override;
    void applyTo(RightMiddleLeg& leg) override;
    void applyTo(RightRearLeg& leg) override;

    const char* getName() const override { return "Backward"; }

    // Toggle direction for oscillation
    void toggleDirection() { _movingToMax = !_movingToMax; }
};

#endif
