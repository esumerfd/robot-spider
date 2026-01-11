#include <right_gait.h>
#include <logging.h>

RightGait::RightGait() : _movingToMax(true) {
  _speed = _board.servoSpeed();
}

void RightGait::applyTo(LeftFrontLeg& leg) {
  // No movement - not targeted by this gait
}

void RightGait::applyTo(LeftMiddleLeg& leg) {
  // Target servo 3 (Left Middle knee)
  // Use safe range with offset from extremes to avoid continuous rotation mode
  const uint16_t offset = 5;
  const uint16_t safeMin = _board.servoMin() + offset;
  const uint16_t safeMax = _board.servoMax() - offset;

  if (_movingToMax) {
    leg.knee().setTarget(safeMax, _speed);
  } else {
    leg.knee().setTarget(safeMin, _speed);
  }
}

void RightGait::applyTo(LeftRearLeg& leg) {
  // No movement - not targeted by this gait
}

void RightGait::applyTo(RightFrontLeg& leg) {
  // No movement - not targeted by this gait
}

void RightGait::applyTo(RightMiddleLeg& leg) {
  // No movement - not targeted by this gait
}

void RightGait::applyTo(RightRearLeg& leg) {
  // No movement - not targeted by this gait
}
