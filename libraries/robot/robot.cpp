
#include <robot.h>
#include <flasher.h>
#include <logging.h>

#include <body.h>
#include <servo.h>
#include <board.h>
#include <sequence.h>
#include <mover_up.h>

Flasher flasher;

Board board;
MoverUp mover = MoverUp(board.servoMin(), board.servoMax());
Servo servo = Servo(board, 0);
Sequence sequence = Sequence(servo);
Body body = Body(servo, sequence);

void Robot::setup() {
  Log::begin();
  flasher.begin();

  // seed initial sequence with movements.

  // Construct model
  // new Knee
  // new Sholder
  // new Leg
  //
  sequence.add(mover);

  body.begin();

  // init pwm

  // init servos to default position
  // report status of memory, devices.

}

void Robot::loop() {
  flasher.flash();

  // check queue for operation.
  // take next opertion step.
  body.action();

  delay(100);
}
