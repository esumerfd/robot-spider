#include <mover_up.h>

MoverUp::MoverUp(uint16_t positionMin, uint16_t positionMax) : _positionMin(positionMin), _positionMax(positionMax) {
  _directionUp = true;
}

uint16_t MoverUp::move(uint16_t currentPosition) {

  uint16_t newPosition = currentPosition;

  if (_directionUp && currentPosition + DISTANCE <= _positionMax) {
    newPosition = currentPosition + DISTANCE;
  }
  else if (!_directionUp && currentPosition - DISTANCE >= _positionMin) {
    newPosition = currentPosition - DISTANCE;
  }
  else {
    _directionUp = !_directionUp;
  }

  return newPosition;
}
