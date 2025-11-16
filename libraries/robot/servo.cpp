#include <servo.h>
#include <logging.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// One instance of PWM for all servos
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

Servo::Servo(Board &board, uint8_t servonum) 
  : _board(board), _servonum(servonum) {}

uint16_t Servo::getPosition() {
  return _position;
}

void Servo::begin() {
  Wire.begin(_board.pwmSDA(), _board.pwmSCL());

  pwm.begin();
  pwm.setPWMFreq(60);
  pwm.setPWM(_servonum, 0, _position);
}

void Servo::move(uint16_t position) {
  _position = position;
  Log::println("Servo: servonum %d, move %d", _servonum, _position);

  pwm.setPWM(_servonum, 0, _position);
}

