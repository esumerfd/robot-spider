#include <one_sweep_sequence.h>
#include <logging.h>

OneSweepSequence::OneSweepSequence() : _movingToMax(true) {
  _speed = _board.servoSpeed();
}

void OneSweepSequence::applySweepToJoint(Joint& joint) {
  // Use safe range with larger offset from extremes to avoid 360° rotation
  // Servos can enter continuous rotation mode near extremes (150, 600)
  // Using 50-unit safety margin: 200-550
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
