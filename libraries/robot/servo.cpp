#include <servo.h>
#include <logging.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// One instance of PWM for all servos
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// Static flag to track PWM initialization
bool Servo::_pwmInitialized = false;

Servo::Servo(Board &board, uint8_t servonum)
  : _board(board), _servonum(servonum) {}

uint16_t Servo::getPosition() {
  return _position;
}

void Servo::initializePWM(Board& board) {
  if (_pwmInitialized) {
    return; // Already initialized
  }

  Wire.begin(board.pwmSDA(), board.pwmSCL());
  pwm.begin();
  pwm.setPWMFreq(60);

  _pwmInitialized = true;
  Log::println("Servo: PWM driver initialized (I2C 0x40, 60Hz)");
}

void Servo::begin() {
  // Set initial position for this servo
  pwm.setPWM(_servonum, 0, _position);
}

void Servo::move(uint16_t position) {
  _position = position;
  // Removed verbose logging - called too frequently during movement
  // Log::println("Servo: servonum %d, move %d", _servonum, _position);

  pwm.setPWM(_servonum, 0, _position);
}

