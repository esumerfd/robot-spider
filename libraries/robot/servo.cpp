#include <servo.h>
#include <logging.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Global PWM driver - matches Adafruit example pattern
static Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Static flag to track PWM initialization
bool Servo::_pwmInitialized = false;

Servo::Servo(Board &board, uint8_t servonum)
  : _board(board), _servonum(servonum) {}

uint16_t Servo::getPosition() {
  return _position;
}

void Servo::initializePWM(Board& board) {
  // Single-threaded check-and-initialize pattern
  // See docs/adr/001-servo-pwm-initialization-thread-safety.md for threading model assumptions
  if (_pwmInitialized) {
    return; // Already initialized
  }

  // ESP32: Initialize I2C with custom pins
  Wire.begin(board.pwmSDA(), board.pwmSCL());

  // Initialize PWM driver (matches Adafruit example)
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // Calibrate oscillator
  pwm.setPWMFreq(50);  // 50 Hz for servos (was 60)

  delay(10);  // Startup delay from example

  _pwmInitialized = true;
  Log::println("Servo: PWM driver initialized");
}

void Servo::begin() {
  // Set initial position for this servo
  pwm.setPWM(_servonum, 0, _position);
}

void Servo::move(uint16_t position) {
  _position = position;

  pwm.setPWM(_servonum, 0, _position);
  delay(100);  // 50ms between calls - no limit
}

