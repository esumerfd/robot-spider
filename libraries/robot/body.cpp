
#include <body.h>

Body::Body(Servo &servo, Mover &mover) 
  : _servo(servo), _mover(mover) {}

void Body::begin() {
  
  _servo.begin();
}

void Body::action() {

  _position = _mover.move(_position);

  _servo.move(_position);
}

