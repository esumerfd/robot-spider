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

  public:

    Body(Servo &servo, Mover &mover);

    void begin();
    void action();
};

#endif
