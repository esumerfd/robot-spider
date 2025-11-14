#ifndef ROBOT_H
#define ROBOT_H

#include <flasher.h>
#include <board.h>
#include <servo.h>
#include <mover_up.h>
#include <sequence.h>
#include <body.h>

class Robot {
  private:
    // Ordered by dependency (board first, then things that depend on it)
    Flasher _flasher;
    Board _board;
    Servo _servo;
    MoverUp _moverUp;
    Sequence _sequence;
    Body _body;

  public:
    Robot();

    void setup();
    void loop();
};

#endif
