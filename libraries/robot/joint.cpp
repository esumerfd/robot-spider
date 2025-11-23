#include <joint.h>
#include <logging.h>
#include <Arduino.h>

Joint::Joint(Servo &servo, uint16_t initialPos)
  : _servo(servo), _currentPos(initialPos), _targetPos(initialPos), _speed(100) {
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

  // Move towards target
  if (_currentPos < _targetPos) {
    uint16_t distance = _targetPos - _currentPos;
    if (maxDelta >= distance) {
      _currentPos = _targetPos; // Reached target
    } else {
      _currentPos += maxDelta;
    }
  } else {
    uint16_t distance = _currentPos - _targetPos;
    if (maxDelta >= distance) {
      _currentPos = _targetPos; // Reached target
    } else {
      _currentPos -= maxDelta;
    }
  }

  // Apply position to servo
  _servo.move(_currentPos);
}

void Joint::setTarget(uint16_t targetPos, uint16_t speed) {
  _targetPos = targetPos;
  _speed = speed;
}
