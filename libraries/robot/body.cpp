#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <body.h>
#include <board.h>

Board board;

uint8_t servonum = 0;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

void Body::begin() {
  
  Wire.begin(board.pwmSDA(), board.pwmSCL());

  pwm.begin();
  pwm.setPWMFreq(60);
  pwm.setPWM(servonum, 0, board.servoMiddle());
}

