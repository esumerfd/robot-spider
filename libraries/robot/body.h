#ifndef BODY_H
#define BODY_H

#include <board.h>
#include <move.h>

class Body {

  private:

    Board _board;
    Move _mover;

  public:

    Body(Board &board, Move &mover);

    void begin();
    void action();
};

#endif
