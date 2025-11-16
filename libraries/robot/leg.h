#ifndef LEG_H
#define LEG_H

#include <shoulder.h>
#include <knee.h>

/*
 * Base class for robot legs.
 *
 * Each leg has a shoulder and knee joint. Named subclasses
 * (LeftFrontLeg, etc.) can override movement methods to
 * provide leg-specific positioning and kinematics.
 */
class Leg {
  protected:
    Shoulder _shoulder;
    Knee _knee;

  public:
    Leg(Servo &shoulderServo, Servo &kneeServo,
        uint16_t shoulderInitial, uint16_t kneeInitial);

    // Update both joints based on elapsed time
    virtual void update(uint32_t deltaMs);

    // Get leg name for debugging/logging
    virtual const char* getName() const = 0;

    // Access to joints
    Shoulder& shoulder() { return _shoulder; }
    Knee& knee() { return _knee; }
};

#endif
