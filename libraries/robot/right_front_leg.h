#ifndef RIGHT_FRONT_LEG_H
#define RIGHT_FRONT_LEG_H

#include <leg.h>

/*
 * Right front leg - specific positioning and movement characteristics.
 */
class RightFrontLeg : public Leg {
  public:
    RightFrontLeg(Servo &shoulderServo, Servo &kneeServo,
                  uint16_t shoulderInitial, uint16_t kneeInitial);

    const char* getName() const override { return "RightFront"; }
};

#endif
