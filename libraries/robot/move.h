#ifndef MOVE_H
#define MOVE_H

#include <board.h>

#define DISTANCE 5

class Move {
  private:

    bool directionUp = true;

  public:
    int position = SERVOMIN;
    void move();
};

#endif MOVE_H

