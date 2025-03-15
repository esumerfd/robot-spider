#include <mover.h>

uint16_t Mover::move(uint16_t currentPosition) {

  uint16_t newPosition = currentPosition;

  if (_directionUp && currentPosition <= _positionMax) {
    newPosition = currentPosition + DISTANCE;
  }
  else if (!_directionUp && currentPosition >= _positionMin) {
    newPosition = currentPosition - DISTANCE;
  }
  else {
    _directionUp = !_directionUp;
  }

  return newPosition;
}
