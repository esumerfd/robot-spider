#ifndef RIGHT_REAR_LEG_H
#define RIGHT_REAR_LEG_H

#include <leg.h>

/*
 * Right rear leg - specific positioning and movement characteristics.
 */
class RightRearLeg : public Leg {
  public:
    RightRearLeg(Servo &shoulderServo, Servo &kneeServo,
                 uint16_t shoulderInitial, uint16_t kneeInitial);

    const char* getName() const override { return "RightRear"; }
};

#endif
