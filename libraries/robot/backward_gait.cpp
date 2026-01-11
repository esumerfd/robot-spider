#include <backward_gait.h>
#include <logging.h>

BackwardGait::BackwardGait() : _movingToMax(true) {
  _speed = _board.servoSpeed();
}

void BackwardGait::applyTo(LeftFrontLeg& leg) {
  // Target servo 1 (Left Front knee)
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

void BackwardGait::applyTo(LeftMiddleLeg& leg) {
  // No movement - not targeted by this gait
}

void BackwardGait::applyTo(LeftRearLeg& leg) {
  // No movement - not targeted by this gait
}

void BackwardGait::applyTo(RightFrontLeg& leg) {
  // No movement - not targeted by this gait
}

void BackwardGait::applyTo(RightMiddleLeg& leg) {
  // No movement - not targeted by this gait
}

void BackwardGait::applyTo(RightRearLeg& leg) {
  // No movement - not targeted by this gait
}
