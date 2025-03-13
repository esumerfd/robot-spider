#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <body.h>
#include <board.h>
#include <move.h>
#include <logging.h>

uint8_t servonum = 0;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

Body::Body(Board &board, Move &mover) : _board(board), _mover(mover) {}

void Body::begin() {
  
  Wire.begin(_board.pwmSDA(), _board.pwmSCL());

  pwm.begin();
  pwm.setPWMFreq(60);
  pwm.setPWM(servonum, 0, _board.servoMiddle());
}

void Body::action() {

  _mover.move();

  pwm.setPWM(servonum, 0, _mover.position);
}

