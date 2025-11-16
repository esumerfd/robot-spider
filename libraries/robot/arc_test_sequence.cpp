#include <arc_test_sequence.h>
#include <logging.h>

ArcTestSequence::ArcTestSequence() {
  // Calculate speed for 2-second arc
  uint16_t range = _board.servoMax() - _board.servoMin();
  _speed = range / 2; // Range / 2 seconds = units per second
}

void ArcTestSequence::applyTo(LeftFrontLeg& leg) {
  // Move shoulder between min and max
  uint16_t currentPos = leg.shoulder().getPosition();
  uint16_t targetPos;

  // Toggle between min and max
  if (currentPos <= _board.servoMiddle()) {
    targetPos = _board.servoMax();
  } else {
    targetPos = _board.servoMin();
  }

  leg.shoulder().setTarget(targetPos, _speed);
  Log::println("ArcTest -> %s shoulder: %d -> %d (speed=%d)",
               leg.getName(), currentPos, targetPos, _speed);
}

void ArcTestSequence::applyTo(LeftMiddleLeg& leg) {
  // For now, only LeftFrontLeg shoulder is connected
  // Other legs do nothing
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
