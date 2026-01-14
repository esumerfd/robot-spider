#include <servo.h>
#include <logging.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Global PWM driver - matches Adafruit example pattern
static Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Static flag to track PWM initialization
bool Servo::_pwmInitialized = false;

Servo::Servo(Board &board, uint8_t servonum)
  : _board(board), _servonum(servonum), _positionAngle(90.0f) {}

float Servo::getPosition() {
  return _positionAngle;
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

  _pwmInitialized = true;
  Log::println("Servo: PWM driver initialized");
}

void Servo::begin() {
  // Set initial position for this servo
  uint16_t pwm_value = _board.angleToPWM(_servonum, _positionAngle);
  pwm.setPWM(_servonum, 0, pwm_value);
}

void Servo::move(float angle) {
  _positionAngle = angle;

  // Convert angle to PWM for hardware
  uint16_t pwm_value = _board.angleToPWM(_servonum, angle);
  pwm.setPWM(_servonum, 0, pwm_value);
  // Note: Blocking delay removed - rate limiting now handled by Joint class
  // via CallRateProfiler to prevent servo spinning while allowing smooth movement
}

