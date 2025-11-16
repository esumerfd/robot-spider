#ifndef LEFT_FRONT_LEG_H
#define LEFT_FRONT_LEG_H

#include <leg.h>

/*
 * Left front leg - specific positioning and movement characteristics.
 */
class LeftFrontLeg : public Leg {
  public:
    LeftFrontLeg(Servo &shoulderServo, Servo &kneeServo,
                 uint16_t shoulderInitial, uint16_t kneeInitial);

    const char* getName() const override { return "LeftFront"; }
};

#endif
