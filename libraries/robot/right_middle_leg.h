#ifndef RIGHT_MIDDLE_LEG_H
#define RIGHT_MIDDLE_LEG_H

#include <leg.h>

/*
 * Right middle leg - specific positioning and movement characteristics.
 */
class RightMiddleLeg : public Leg {
  public:
    RightMiddleLeg(Servo &shoulderServo, Servo &kneeServo,
                   uint16_t shoulderInitial, uint16_t kneeInitial);

    const char* getName() const override { return "RightMiddle"; }
};

#endif
