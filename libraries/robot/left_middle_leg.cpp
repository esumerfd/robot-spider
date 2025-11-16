#include <left_middle_leg.h>

LeftMiddleLeg::LeftMiddleLeg(Servo &shoulderServo, Servo &kneeServo,
                             uint16_t shoulderInitial, uint16_t kneeInitial)
  : Leg(shoulderServo, kneeServo, shoulderInitial, kneeInitial) {
}
