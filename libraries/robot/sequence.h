#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <logging.h>
#include <servo.h>
#include <operation.h>
#include <vector>

class Sequence {
  private:
    std::vector<Operation *> _sequence; // Store pointers to Operation objects
    Servo &_servo;

  public:
    Sequence(Servo &servo);

    void add(Operation &operation);
    size_t size();
    void move();
};

#endif


