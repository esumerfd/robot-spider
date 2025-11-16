#include <shoulder.h>

Shoulder::Shoulder(Servo &servo, uint16_t initialPos)
  : Joint(servo, initialPos) {
}
