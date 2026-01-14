#include <joint.h>
#include <logging.h>
#include <Arduino.h>

Joint::Joint(Servo &servo, uint16_t initialPos)
  : _servo(servo),
    _currentPos(initialPos),
    _targetPos(initialPos),
    _speed(100),
    _servoWriteProfiler("ServoWrite", false, 1000, 20) {  // 20ms min interval (50Hz max)
}

void Joint::update(uint32_t deltaMs) {
  if (_currentPos == _targetPos) {
    return; // Already at target
  }

  // Calculate maximum distance we can move in this time step
  uint32_t maxDelta = (_speed * deltaMs) / 1000;

  if (maxDelta == 0) {
    return; // Time step too small
  }

  // Calculate distance and direction to target
  int16_t direction = (_currentPos < _targetPos) ? 1 : -1;
  uint16_t distance = abs((int16_t)_targetPos - (int16_t)_currentPos);

  // Move towards target or reach it
  if (maxDelta >= distance) {
    _currentPos = _targetPos; // Reached target
  } else {
    _currentPos += direction * maxDelta;
  }

  Log::println("Joint: cur=%d tgt=%d speed=%d deltaMs=%d", _currentPos, _targetPos, _speed, deltaMs);

  // Apply position to servo with rate limiting
  uint16_t positionToWrite = _currentPos;
  _servoWriteProfiler.executeIfReady(millis(), [this, positionToWrite]() {
    _servo.move(positionToWrite);
  });
}

void Joint::setTarget(uint16_t targetPos, uint16_t speed) {
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
