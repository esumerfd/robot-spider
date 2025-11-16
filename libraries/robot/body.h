#ifndef BODY_H
#define BODY_H

#include <servo.h>
#include <mover_up.h>
#include <sequence.h>

/*
 * Composes all the parts of the body and how that are controlled.
 */
class Body {

  private:

    Servo &_servo;
    Sequence &_sequence;

  public:

    Body(Servo &servo, Sequence &sequence);

    void begin();
    void action();
};

#endif
