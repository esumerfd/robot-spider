#include <right_rear_leg.h>

RightRearLeg::RightRearLeg(Servo &shoulderServo, Servo &kneeServo,
                           uint16_t shoulderInitial, uint16_t kneeInitial)
  : Leg(shoulderServo, kneeServo, shoulderInitial, kneeInitial) {
}
