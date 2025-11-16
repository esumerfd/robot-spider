#include <left_rear_leg.h>

LeftRearLeg::LeftRearLeg(Servo &shoulderServo, Servo &kneeServo,
                         uint16_t shoulderInitial, uint16_t kneeInitial)
  : Leg(shoulderServo, kneeServo, shoulderInitial, kneeInitial) {
}
