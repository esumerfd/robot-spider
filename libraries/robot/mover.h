#ifndef MOVER_H
#define MOVER_H

#include <stdint.h>

/*
 * A mover is responsible for applying a delta change to
 * a servo state.
 */
class Mover {
  public:

    virtual uint16_t move(uint16_t currentPosition) = 0;
};

#endif MOVER_H

