#ifndef SHOULDER_H
#define SHOULDER_H

#include <joint.h>

/*
 * Shoulder joint - controls leg lifting and positioning.
 */
class Shoulder : public Joint {
  public:
    Shoulder(Servo &servo, uint16_t initialPos);
};

#endif
