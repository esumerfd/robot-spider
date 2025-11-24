#ifndef BODY_H
#define BODY_H

#include <left_front_leg.h>
#include <left_middle_leg.h>
#include <left_rear_leg.h>
#include <right_front_leg.h>
#include <right_middle_leg.h>
#include <right_rear_leg.h>
#include <gait_sequence.h>

/*
 * Composes all the parts of the body - 6 named legs.
 *
 * Body owns all servos and legs. It coordinates movement by
 * applying gait sequences to the legs and updating them based
 * on elapsed time.
 */
class Body {
  private:
    Board& _board;

    // All 12 servos (2 per leg)
    Servo _leftFrontShoulder;
    Servo _leftFrontKnee;
    Servo _leftMiddleShoulder;
    Servo _leftMiddleKnee;
    Servo _leftRearShoulder;
    Servo _leftRearKnee;
    Servo _rightFrontShoulder;
    Servo _rightFrontKnee;
    Servo _rightMiddleShoulder;
    Servo _rightMiddleKnee;
    Servo _rightRearShoulder;
    Servo _rightRearKnee;

    // All 6 legs
    LeftFrontLeg _leftFront;
    LeftMiddleLeg _leftMiddle;
    LeftRearLeg _leftRear;
    RightFrontLeg _rightFront;
    RightMiddleLeg _rightMiddle;
    RightRearLeg _rightRear;

    // Arrays for iteration (initialized in constructor)
    static const int SERVO_COUNT = 12;
    static const int LEG_COUNT = 6;
    Servo* _servos[SERVO_COUNT];
    Leg* _legs[LEG_COUNT];

  public:
    Body(Board& board);

    void begin();
    void update(uint32_t deltaMs);

    // Apply a gait sequence to all legs
    void applyGait(GaitSequence& gait);

    // Access to individual legs (for testing/debugging)
    LeftFrontLeg& leftFront() { return _leftFront; }
    LeftMiddleLeg& leftMiddle() { return _leftMiddle; }
    LeftRearLeg& leftRear() { return _leftRear; }
    RightFrontLeg& rightFront() { return _rightFront; }
    RightMiddleLeg& rightMiddle() { return _rightMiddle; }
    RightRearLeg& rightRear() { return _rightRear; }

    // Check if all legs have reached their targets
    bool atTarget() const;
};

#endif
