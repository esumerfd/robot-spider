#ifndef BODY_H
#define BODY_H

#include <Arduino.h>
#include <left_front_leg.h>
#include <left_middle_leg.h>
#include <left_rear_leg.h>
#include <right_front_leg.h>
#include <right_middle_leg.h>
#include <right_rear_leg.h>
#include <gait_sequence.h>
#include <i_gait_target.h>

/*
 * Composes all the parts of the body - 6 named legs.
 *
 * Body owns all servos and legs. It coordinates movement by
 * applying gait sequences to the legs and updating them based
 * on elapsed time.
 *
 * Implements IGaitTarget for use with gait sequencing and testing.
 */
class Body : public IGaitTarget {
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

    // IGaitTarget interface implementation
    void update(uint32_t deltaMs) override;
    void applyGait(GaitSequence& gait) override;
    bool atTarget() const override;
    void resetToMiddle() override;
    void logState() const override;

    // Access to individual legs (for testing/debugging)
    LeftFrontLeg& leftFront() { return _leftFront; }
    LeftMiddleLeg& leftMiddle() { return _leftMiddle; }
    LeftRearLeg& leftRear() { return _leftRear; }
    RightFrontLeg& rightFront() { return _rightFront; }
    RightMiddleLeg& rightMiddle() { return _rightMiddle; }
    RightRearLeg& rightRear() { return _rightRear; }

    // Diagnostic: wiggle a servo by name to test connectivity
    // Returns true if servo name was valid, false otherwise
    bool wiggleServo(const String& servoName);
};

#endif
