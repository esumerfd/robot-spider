#ifndef KNEE_H
#define KNEE_H

#include <joint.h>

/*
 * Knee joint - controls leg extension and retraction.
 */
class Knee : public Joint {
  public:
    Knee(Servo &servo, uint16_t initialPos);
};

#endif
