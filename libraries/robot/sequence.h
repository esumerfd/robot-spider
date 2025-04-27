#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <mover_up.h>
#include <vector>

class Sequence {
  private:

    std::vector<MoverUp> _sequence;

  public:

    void add(MoverUp& mover);
    size_t size();
};

#endif 


