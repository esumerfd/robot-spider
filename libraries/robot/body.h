#ifndef BODY_H
#define BODY_H

#include <board.h>

class Body {

  private:

    Board _board;

  public:

    Body(Board &board);
    void begin();
};

#endif
