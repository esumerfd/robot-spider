#ifndef MOVER_UP_H
#define MOVER_UP_H

#include <stdint.h>

#define DISTANCE 5

/*
 * A mover is responsible for applying a delta change to position.
 * 
 * Not coupled with servos, just a transformation of position.
 */
class MoverUp {
  private:

    bool _directionUp;

    uint16_t _positionMin;
    uint16_t _positionMax;

  public:

    MoverUp(uint16_t positionMin, uint16_t positionMax);

    uint16_t move(uint16_t currentPosition);

    bool isDirectionUp() {
      return _directionUp;
    }
};

#endif 

