#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <board.h>

/*
 * Simple abstraction of servo library
 */
class Servo {
  private:

    Board _board;
    uint8_t _servonum = 0;

  public:

    Servo(Board &board, uint8_t servonum);

    void begin();
    void move(uint16_t position);
};

#endif
