#ifndef BODY_H
#define BODY_H

#include <servo.h>
#include <mover_up.h>

/*
 * Composes all the parts of the body and how that are controlled.
 */
class Body {

  private:

    Servo _servo;
    MoverUp& _mover;

  public:

    Body(Servo &servo, MoverUp& mover);

    void begin();
    void action();
};

#endif
