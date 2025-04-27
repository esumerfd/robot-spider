#include <sequence.h>

void Sequence::add(MoverUp& mover) {
  _sequence.push_back(mover);
}

size_t Sequence::size() {
  return _sequence.size();
}

