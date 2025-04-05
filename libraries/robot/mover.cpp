#include <stdio.h>
#include <string.h>
#include <mover.h>
#include <logging.h>

Mover::Mover() : _directionUp(true) {}

uint16_t Mover::move(uint16_t currentPosition) {

  uint16_t newPosition = currentPosition;

  char branch[20] = "undefined";

  if (_directionUp && currentPosition + DISTANCE < _positionMax) {
    newPosition = currentPosition + DISTANCE;
    strcpy(branch,  "moved up");
  }
  else if (!_directionUp && currentPosition - DISTANCE > _positionMin) {
    newPosition = currentPosition - DISTANCE;
    strcpy(branch, "moved down");
  }
  else {
    _directionUp = !_directionUp;
    strcpy(branch, "changed");
  }

  bool x = (!_directionUp && currentPosition - DISTANCE > _positionMin);
    
  Log::println("Mover (%d/%d) %d: %d => %d: %s : %d", 
    _positionMin, _positionMax, 
    _directionUp, currentPosition, newPosition,
    branch, x);

  return newPosition;
}
