#include <leg.h>

Leg::Leg(Servo &shoulderServo, Servo &kneeServo,
         float shoulderInitial, float kneeInitial)
  : _shoulder(shoulderServo, shoulderInitial),
    _knee(kneeServo, kneeInitial) {
}

void Leg::update(uint32_t deltaMs) {
  _shoulder.update(deltaMs);
  _knee.update(deltaMs);
}
