#ifndef ARC_TEST_SEQUENCE_H
#define ARC_TEST_SEQUENCE_H

#include <gait_sequence.h>
#include <board.h>

/*
 * Simple test sequence that moves servos in a 2-second arc.
 *
 * This is a stateless movement description:
 * - Moves from min to max position
 * - Takes 2 seconds to complete one arc
 * - Can be reapplied to continue the movement
 *
 * Speed calculation:
 * - Range: SERVOMAX (600) - SERVOMIN (150) = 450 units
 * - Duration: 2000ms = 2 seconds
 * - Speed: 450 units / 2 seconds = 225 units/second
 */
class ArcTestSequence : public GaitSequence {
  private:
    Board _board;
    uint16_t _speed;

  public:
    ArcTestSequence();

    void applyTo(LeftFrontLeg& leg) override;
    void applyTo(LeftMiddleLeg& leg) override;
    void applyTo(LeftRearLeg& leg) override;
    void applyTo(RightFrontLeg& leg) override;
    void applyTo(RightMiddleLeg& leg) override;
    void applyTo(RightRearLeg& leg) override;

    const char* getName() const override { return "ArcTest"; }
};

#endif
