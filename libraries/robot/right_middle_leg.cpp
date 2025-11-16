#include <right_middle_leg.h>

RightMiddleLeg::RightMiddleLeg(Servo &shoulderServo, Servo &kneeServo,
                               uint16_t shoulderInitial, uint16_t kneeInitial)
  : Leg(shoulderServo, kneeServo, shoulderInitial, kneeInitial) {
}
