#include <left_front_leg.h>

LeftFrontLeg::LeftFrontLeg(Servo &shoulderServo, Servo &kneeServo,
                           uint16_t shoulderInitial, uint16_t kneeInitial)
  : Leg(shoulderServo, kneeServo, shoulderInitial, kneeInitial) {
}
