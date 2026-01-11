#include <left_gait.h>
#include <logging.h>

LeftGait::LeftGait() : _movingToMax(true) {
  // Calculate speed for 2-second sweep
  _speed = _board.servoRange() / 2;  // Range / 2 seconds = units per second
}

void LeftGait::applyTo(LeftFrontLeg& leg) {
  // No movement - not targeted by this gait
}

void LeftGait::applyTo(LeftMiddleLeg& leg) {
  // Target servo 2 (Left Middle shoulder)
  // Use safe range with offset from extremes to avoid continuous rotation mode
  const uint16_t offset = 5;
  const uint16_t safeMin = _board.servoMin() + offset;
  const uint16_t safeMax = _board.servoMax() - offset;

  if (_movingToMax) {
    leg.shoulder().setTarget(safeMax, _speed);
  } else {
    leg.shoulder().setTarget(safeMin, _speed);
  }
}

void LeftGait::applyTo(LeftRearLeg& leg) {
  // No movement - not targeted by this gait
}

void LeftGait::applyTo(RightFrontLeg& leg) {
  // No movement - not targeted by this gait
}

void LeftGait::applyTo(RightMiddleLeg& leg) {
  // No movement - not targeted by this gait
}

void LeftGait::applyTo(RightRearLeg& leg) {
  // No movement - not targeted by this gait
}
