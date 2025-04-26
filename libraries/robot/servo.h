#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <board.h>
#include <mover_up.h>

/*
 * Simple abstraction of servo library
 */
class Servo {
  private:

    Board _board;

    uint8_t _servonum = 0;
    uint16_t _position = SERVOMIN;

  public:

    Servo(Board &board, uint8_t servonum);

    void begin();
    void move(MoverUp& mover);
};

#endif
