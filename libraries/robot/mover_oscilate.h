#ifndef MOVER_OSCILATE_H
#define MOVER_OSCILATE_H

#include <stdint.h>
#include <mover.h>

#define DISTANCE 5

/*
 * A mover is responsible for applying a delta change to
 * a servo state.
 */
class MoverOscilate : public Mover {
  private:

    bool _directionUp = true;

    uint16_t _positionMin;
    uint16_t _positionMax;

  public:

    MoverOscilate(uint16_t positionMin, uint16_t positionMax)
      : _positionMin(positionMin), _positionMax(positionMax) {}

    uint16_t move(uint16_t currentPosition);
};

#endif

