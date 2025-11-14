#ifndef OPERATION_H
#define OPERATION_H

#include <stdint.h>

/*
 * Base interface for movement operations.
 *
 * Operations transform servo positions without being coupled to specific servos.
 * Each operation takes a current position and returns the next position.
 */
class Operation {
  public:
    virtual uint16_t move(uint16_t currentPosition) = 0;
    virtual ~Operation() = default;
};

#endif
