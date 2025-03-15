#ifndef MOVE_H
#define MOVE_H

#include <board.h>

#define DISTANCE 5

/*
 * A mover is responsible for applying a delate change to
 * a servo state.
 */
class Move {
  private:

    bool directionUp = true;

  public:
    int position = SERVOMIN;
    void move();
};

#endif MOVE_H

