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

  // Apply position to servo
  _servo.move(_currentPos);
}

void Joint::setTarget(uint16_t targetPos, uint16_t speed) {
  _targetPos = targetPos;
  _speed = speed;
}
