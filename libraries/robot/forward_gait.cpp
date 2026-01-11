#include <forward_gait.h>
#include <logging.h>

ForwardGait::ForwardGait() : _movingToMax(true) {
  _speed = _board.servoSpeed();
}

void ForwardGait::applyTo(LeftFrontLeg& leg) {
  // Target servo 0 (Left Front shoulder)
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

void ForwardGait::applyTo(LeftMiddleLeg& leg) {
  // No movement - not targeted by this gait
}

void ForwardGait::applyTo(LeftRearLeg& leg) {
  // No movement - not targeted by this gait
}

void ForwardGait::applyTo(RightFrontLeg& leg) {
  // No movement - not targeted by this gait
}

void ForwardGait::applyTo(RightMiddleLeg& leg) {
  // No movement - not targeted by this gait
}

void ForwardGait::applyTo(RightRearLeg& leg) {
  // No movement - not targeted by this gait
}
