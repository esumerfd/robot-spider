#ifndef MOVER_H
#define MOVER_H

#include <board.h>

#define DISTANCE 5

/*
 * A mover is responsible for applying a delta change to
 * a servo state.
 */
class Mover {
  private:

    bool _directionUp = true;

  public:
    int move(int position);
};

#endif MOVER_H

