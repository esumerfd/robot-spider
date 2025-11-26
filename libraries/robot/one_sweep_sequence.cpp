#include <one_sweep_sequence.h>
#include <logging.h>

OneSweepSequence::OneSweepSequence() : _movingToMax(true) {
  // Calculate speed for 2-second sweep
  uint16_t range = _board.servoMax() - _board.servoMin();
  _speed = range / 2; // Range / 2 seconds = units per second
}

void OneSweepSequence::applySweepToJoint(Joint& joint) {
  // Use safe range with 2-unit offset from extremes to avoid 360° rotation
  // Safe range: 152-598 (2-unit offset from 150-600)
  const uint16_t offset = 5;
  const uint16_t safeMin = _board.servoMin() + offset;
  const uint16_t safeMax = _board.servoMax() - offset;

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
