#include <mover.h>
#include <board.h>

int Mover::move(int currentPosition) {

  int newPosition = currentPosition;

  if (_directionUp && currentPosition <= SERVOMAX) {
    newPosition = currentPosition + DISTANCE;
  }
  else if (!_directionUp && currentPosition >= SERVOMIN) {
    newPosition = currentPosition - DISTANCE;
  }
  else {
    _directionUp = !_directionUp;
  }

  return newPosition;
}
