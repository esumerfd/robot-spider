#include <one_sweep_sequence.h>
#include <logging.h>

OneSweepSequence::OneSweepSequence() : _movingToMax(true) {
  _speed = _board.servoSpeed();
}

void OneSweepSequence::applySweepToJoint(Joint& joint) {
  // Use safe angle range with offset from extremes to avoid servo issues
  // Using 2-degree safety margin from 0-180 degree range
  const float offset = 2.0f;
  const float safeMin = _board.servoMin() + offset;  // 2.0 degrees
  const float safeMax = _board.servoMax() - offset;  // 178.0 degrees

  if (_movingToMax) {
    joint.setTarget(safeMax, _speed);
  } else {
    joint.setTarget(safeMin, _speed);
  }
}

void OneSweepSequence::applyTo(LeftFrontLeg& leg) {
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}

void OneSweepSequence::applyTo(LeftMiddleLeg& leg) {
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}

void OneSweepSequence::applyTo(LeftRearLeg& leg) {
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}

void OneSweepSequence::applyTo(RightFrontLeg& leg) {
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}

void OneSweepSequence::applyTo(RightMiddleLeg& leg) {
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}

void OneSweepSequence::applyTo(RightRearLeg& leg) {
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}
