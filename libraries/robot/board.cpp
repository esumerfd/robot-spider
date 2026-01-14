#include <board.h>
#include <Arduino.h>

#define I2C_SDA 15
#define I2C_SCL 14

// Calibration offsets for each servo (in degrees)
// All zeros initially - adjust based on physical servo alignment
const int8_t Board::SERVO_CALIBRATION_OFFSETS[12] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int Board::pwmSDA() {
  return I2C_SDA;
}

int Board::pwmSCL() {
  return I2C_SCL;
}

float Board::servoMin() {
  return 0.0f;
}

float Board::servoMax() {
  return 180.0f;
}

float Board::servoRange() {
  return 180.0f;
}

float Board::servoSpeed() {
  return 180.0f;  // Full range (180 degrees) in 1s
}

float Board::servoSpeed(uint16_t durationMs, float distance) {
  // Duration = 0 means use constant speed
  if (durationMs == 0) {
    return servoSpeed();  // Default constant speed (180°/s)
  }

  // Calculate speed needed to cover distance in given duration
  // speed (°/s) = distance (°) / time (s)
  // time (s) = durationMs / 1000
  // Therefore: speed = (distance * 1000) / durationMs
  float calculatedSpeed = (distance * 1000.0f) / (float)durationMs;

  // Physical limit: servos can do ~180° in 0.3s = 600°/s max
  // Clamp to safe maximum
  const float MAX_SERVO_SPEED = 600.0f;  // degrees per second
  if (calculatedSpeed > MAX_SERVO_SPEED) {
    return MAX_SERVO_SPEED;
  }

  return calculatedSpeed;
}

float Board::servoMiddle() {
  return 90.0f;
}

int8_t Board::getServoCalibrationOffset(uint8_t servoNum) const {
  if (servoNum >= 12) return 0;
  return SERVO_CALIBRATION_OFFSETS[servoNum];
}

uint16_t Board::angleToPWM(uint8_t servoNum, float angle) const {
  // Apply calibration offset
  float calibratedAngle = angle + getServoCalibrationOffset(servoNum);

  // Clamp to valid range
  calibratedAngle = constrain(calibratedAngle, 0.0f, 180.0f);

  // Convert to PWM (150 + angle * 395/180)
  // Formula: PWM = SERVOMIN + (angle * (SERVOMAX - SERVOMIN) / 180)
  uint16_t pwm = SERVOMIN + (uint16_t)(calibratedAngle * 395.0f / 180.0f);

  // Safety clamp to prevent extreme positions
  return constrain(pwm, SERVOMIN + 5, SERVOMAX - 5);
}

float Board::pwmToAngle(uint8_t servoNum, uint16_t pwm) const {
  // Convert PWM to uncalibrated angle
  // Formula: angle = (PWM - SERVOMIN) * 180 / (SERVOMAX - SERVOMIN)
  float angle = (float)(pwm - SERVOMIN) * 180.0f / 395.0f;

  // Remove calibration offset
  angle -= getServoCalibrationOffset(servoNum);

  return angle;
}
