#include <joint.h>
#include <logging.h>
#include <Arduino.h>

// Map servo pin number to descriptive name
static const char* getJointName(uint8_t pin) {
  static const char* names[] = {
    "LF.Shoulder",  // 0
    "LF.Knee",      // 1
    "LM.Shoulder",  // 2
    "LM.Knee",      // 3
    "LR.Shoulder",  // 4
    "LR.Knee",      // 5
    "RF.Shoulder",  // 6
    "RF.Knee",      // 7
    "RM.Shoulder",  // 8
    "RM.Knee",      // 9
    "RR.Shoulder",  // 10
    "RR.Knee"       // 11
  };
  if (pin < 12) return names[pin];
  return "Unknown";
}

Joint::Joint(Servo &servo, float initialPos)
  : _servo(servo),
    _currentPos(initialPos),
    _targetPos(initialPos),
    _speed(90.0f),  // Default 90 degrees per second
    _servoWriteProfiler("ServoWrite", false, 1000, 20) {  // 20ms min interval (50Hz max)
}

void Joint::update(uint32_t deltaMs) {
  if (abs(_currentPos - _targetPos) < 0.5f) {
    return; // Already at target (within tolerance)
  }

  // Calculate maximum distance we can move in this time step
  float maxDelta = (_speed * deltaMs) / 1000.0f;

  if (maxDelta < 0.01f) {
    return; // Time step too small
  }

  // Calculate distance and direction to target
  float direction = (_currentPos < _targetPos) ? 1.0f : -1.0f;
  float distance = abs(_targetPos - _currentPos);

  // Move towards target or reach it
  if (maxDelta >= distance) {
    _currentPos = _targetPos; // Reached target
  } else {
    _currentPos += direction * maxDelta;
  }

  // Apply position to servo with rate limiting
  float positionToWrite = _currentPos;
  _servoWriteProfiler.executeIfReady(millis(), [this, positionToWrite]() {
    _servo.move(positionToWrite);
  });
}

void Joint::setTarget(float targetPos, float speed) {
  // Only log if target actually changed
  if (abs(_targetPos - targetPos) > 0.5f) {
    uint8_t pin = _servo.getServoNum();
    Log::println("%s[%d]: %.1f° -> %.1f° (delta=%.1f°)",
                 getJointName(pin), pin, _currentPos, targetPos, targetPos - _currentPos);
  }
  _targetPos = targetPos;
  _speed = speed;
}

void Joint::enableServoWriteProfiling(bool enabled) {
  _servoWriteProfiler.setEnabled(enabled);
}

void Joint::setServoWriteRateLimit(uint32_t minIntervalMs) {
  // Note: This would require modifying CallRateProfiler to support changing
  // minIntervalMs after construction, which is not currently implemented.
  // For now, the rate limit is set in the constructor (20ms).
  Log::println("Joint: Rate limit change not yet implemented (currently fixed at 20ms)");
}

CallRateProfiler& Joint::getServoWriteProfiler() {
  return _servoWriteProfiler;
}
