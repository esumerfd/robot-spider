#include <robot.h>
#include <logging.h>

// Constructor with member initializer list (guarantees correct order)
Robot::Robot()
  : _flasher(),
    _board(),
    _servo(_board, 0),
    _moverUp(_board.servoMin(), _board.servoMax()),
    _sequence(_servo),
    _body(_servo, _sequence) {
  // All members constructed in declaration order
}

void Robot::setup() {
  Log::begin();
  _flasher.begin();

  delay(500);

  // Construct model
  // new Knee
  // new Sholder
  // new Leg

  _sequence.add(_moverUp);

  _body.begin();

  // report status of memory, devices.
}

void Robot::loop() {
  _flasher.flash();

  // check queue for operation.
  // take next opertion step.
  _body.action();

  // Log::println(
  //     "Robot setup: body: 0x%x (%d), sequence: 0x%x (%d), mover: 0x%x (%d), servo: 0x%x (%d)",
  //     &_body, sizeof(Body), &_sequence, sizeof(Sequence),
  //     &_moverUp, sizeof(MoverUp), &_servo, sizeof(Servo));

  Log::println("Status: mover count %d", _sequence.size());

  delay(100);
}
