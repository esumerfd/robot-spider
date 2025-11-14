#include <sequence.h>

Sequence::Sequence(Servo &servo) : _servo(servo) {}

void Sequence::add(Operation &operation) {
  _sequence.push_back(&operation); // Store a pointer to the Operation object
}

size_t Sequence::size() {
  return _sequence.size();
}

void Sequence::move() {
  uint16_t currentPosition = _servo.getPosition();
  for (Operation *operation : _sequence) {
    currentPosition = operation->move(currentPosition); // Get the next position
    _servo.move(currentPosition); // Apply the position to the servo
  }
}
