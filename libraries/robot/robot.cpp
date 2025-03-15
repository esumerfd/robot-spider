
#include <robot.h>
#include <flasher.h>
#include <logging.h>

#include <body.h>
#include <servo.h>
#include <board.h>
#include <mover.h>

Flasher flasher;

Board board;
Mover mover = Mover(board.servoMin(), board.servoMax());
Servo servo = Servo(board, 0);
Body body = Body(servo, mover);

void Robot::setup() {
  Log::begin();
  flasher.begin(1000);

  // seed initial sequence with movements.

  // Construct model
  // new Knee
  // new Sholder
  // new Leg
  body.begin();

  // init pwm
  Log::println("PWM Init");

  // init servos to default position
  // report status of memory, devices.

}

void Robot::loop() {
  Log::println("Loop -----------------");
  flasher.flash();

  // check queue for operation.
  // take next opertion step.
  body.action();

  delay(250);
}
