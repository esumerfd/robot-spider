#ifndef LEFT_MIDDLE_LEG_H
#define LEFT_MIDDLE_LEG_H

#include <leg.h>

/*
 * Left middle leg - specific positioning and movement characteristics.
 */
class LeftMiddleLeg : public Leg {
  public:
    LeftMiddleLeg(Servo &shoulderServo, Servo &kneeServo,
                  uint16_t shoulderInitial, uint16_t kneeInitial);

    const char* getName() const override { return "LeftMiddle"; }
};

#endif
