#include <arc_test_sequence.h>
#include <logging.h>

ArcTestSequence::ArcTestSequence() {
  // Calculate speed for 2-second arc
  uint16_t range = _board.servoMax() - _board.servoMin();
  _speed = range / 2; // Range / 2 seconds = units per second
}

void ArcTestSequence::applyArcToJoint(Joint& joint, const char* legName, const char* jointName) {
  uint16_t currentPos = joint.getPosition();
  uint16_t targetPos;

  // Toggle between min and max based on current position
  // This ensures all joints move in the same direction (parallel movement)
  if (currentPos <= _board.servoMiddle()) {
    targetPos = _board.servoMax();
  } else {
    targetPos = _board.servoMin();
  }

  joint.setTarget(targetPos, _speed);
  Log::println("ArcTest -> %s %s: %d -> %d (speed=%d)",
               legName, jointName, currentPos, targetPos, _speed);
}

void ArcTestSequence::applyTo(LeftFrontLeg& leg) {
  applyArcToJoint(leg.shoulder(), leg.getName(), "shoulder");
  applyArcToJoint(leg.knee(), leg.getName(), "knee");
}

void ArcTestSequence::applyTo(LeftMiddleLeg& leg) {
  applyArcToJoint(leg.shoulder(), leg.getName(), "shoulder");
  applyArcToJoint(leg.knee(), leg.getName(), "knee");
}

void ArcTestSequence::applyTo(LeftRearLeg& leg) {
  // Not connected yet
}

void ArcTestSequence::applyTo(RightFrontLeg& leg) {
  // Not connected yet
}

void ArcTestSequence::applyTo(RightMiddleLeg& leg) {
  // Not connected yet
}

void ArcTestSequence::applyTo(RightRearLeg& leg) {
  // Not connected yet
}
