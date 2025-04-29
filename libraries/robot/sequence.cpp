#include <sequence.h>

Sequence::Sequence(Servo &servo) : _servo(servo) { }

void Sequence::add(MoverUp &mover) {
  _sequence.push_back(mover);
}

size_t Sequence::size() {
  return _sequence.size();
}

void Sequence::move() {
  for (MoverUp &mover : _sequence) {
    _servo.move(mover);
  }
}
