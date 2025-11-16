#include <knee.h>

Knee::Knee(Servo &servo, uint16_t initialPos)
  : Joint(servo, initialPos) {
}
