#ifndef BODY_H
#define BODY_H

#include <board.h>
#include <mover.h>

/*
 * Composes all the parts of the body and how that are controlled.
 */
class Body {

  private:

    Board _board;
    Mover _mover;

    int _position = SERVOMIN;

  public:

    Body(Board &board, Mover &mover);

    void begin();
    void action();
};

#endif
