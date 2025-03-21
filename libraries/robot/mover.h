#ifndef MOVER_H
#define MOVER_H

#include <stdint.h>

#define DISTANCE 5

/*
 * A mover is responsible for applying a delta change to
 * a servo state.
 */
class Mover {
  private:

    bool _directionUp = true;

    uint16_t _positionMin;
    uint16_t _positionMax;

  public:

    Mover(uint16_t positionMin, uint16_t positionMax)
      : _positionMin(positionMin), _positionMax(positionMax) {}

    uint16_t move(uint16_t currentPosition);
};

#endif 

