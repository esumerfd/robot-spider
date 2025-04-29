#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <servo.h>
#include <mover_up.h>
#include <vector>

class Sequence {
  private:

    std::vector<MoverUp> _sequence;

    Servo &_servo;

  public:

    Sequence(Servo &servo);

    void add(MoverUp &mover);
    size_t size();
    void move();
};

#endif 


