#ifndef BODY_H
#define BODY_H

#include <servo.h>
#include <mover.h>

/*
 * Composes all the parts of the body and how that are controlled.
 */
class Body {

  private:

    Servo _servo;
    Mover _mover;

    uint16_t _position = SERVOMIN;

  public:

    Body(Servo &servo, Mover &mover);

    void begin();
    void action();
};

#endif
