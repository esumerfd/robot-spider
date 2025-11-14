#include <mover_up.h>

MoverUp::MoverUp(uint16_t positionMin, uint16_t positionMax)
    : _positionMin(positionMin), _positionMax(positionMax), _directionUp(true) {}

uint16_t MoverUp::move(uint16_t currentPosition) {
    if (_directionUp) {
        currentPosition += DISTANCE;
        if (currentPosition >= _positionMax) {
            currentPosition = _positionMax;
            _directionUp = false;
        }
    } else {
        currentPosition -= DISTANCE;
        if (currentPosition <= _positionMin) {
            currentPosition = _positionMin;
            _directionUp = true;
        }
    }
    return currentPosition;
}
