#include <right_front_leg.h>

RightFrontLeg::RightFrontLeg(Servo &shoulderServo, Servo &kneeServo,
                             uint16_t shoulderInitial, uint16_t kneeInitial)
  : Leg(shoulderServo, kneeServo, shoulderInitial, kneeInitial) {
}
