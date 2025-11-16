#ifndef LEFT_REAR_LEG_H
#define LEFT_REAR_LEG_H

#include <leg.h>

/*
 * Left rear leg - specific positioning and movement characteristics.
 */
class LeftRearLeg : public Leg {
  public:
    LeftRearLeg(Servo &shoulderServo, Servo &kneeServo,
                uint16_t shoulderInitial, uint16_t kneeInitial);

    const char* getName() const override { return "LeftRear"; }
};

#endif
