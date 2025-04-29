#include <body.h>

Body::Body(Servo &servo, Sequence &sequence) 
  : _servo(servo), _sequence(sequence) {}

void Body::begin() {
  
}

void Body::action() {

  _sequence.move();
}

