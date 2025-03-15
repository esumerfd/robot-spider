#include <servo.h>
#include <logging.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// One instance of PWM for all servos
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

Servo::Servo(Board &board, uint8_t servonum) 
  : _board(board), _servonum(servonum) {}

void Servo::begin() {
  Wire.begin(_board.pwmSDA(), _board.pwmSCL());

  pwm.begin();
  pwm.setPWMFreq(60);
  pwm.setPWM(_servonum, 0, _board.servoMiddle());
}

void Servo::move(uint16_t position) {

  Log::println("Move %d", position);

  pwm.setPWM(_servonum, 0, position);
}
