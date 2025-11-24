#include <one_sweep_sequence.h>
#include <logging.h>

OneSweepSequence::OneSweepSequence() {
  // Calculate speed for 2-second sweep
  uint16_t range = _board.servoMax() - _board.servoMin();
  _speed = range / 2; // Range / 2 seconds = units per second
}

void OneSweepSequence::applySweepToJoint(Joint& joint) {
  // Set target to max position
  joint.setTarget(_board.servoMax(), _speed);
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
  // Not connected yet - but set targets so atTarget() works correctly
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}

void OneSweepSequence::applyTo(RightFrontLeg& leg) {
  // Not connected yet - but set targets so atTarget() works correctly
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}

void OneSweepSequence::applyTo(RightMiddleLeg& leg) {
  // Not connected yet - but set targets so atTarget() works correctly
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}

void OneSweepSequence::applyTo(RightRearLeg& leg) {
  // Not connected yet - but set targets so atTarget() works correctly
  applySweepToJoint(leg.shoulder());
  applySweepToJoint(leg.knee());
}
