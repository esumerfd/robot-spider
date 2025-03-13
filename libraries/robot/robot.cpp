
#include <robot.h>
#include <flasher.h>
#include <logging.h>

#include <body.h>
#include <board.h>
#include <move.h>

Flasher flasher;

Move move;
Board board;
Body body = Body(board, move);

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
  Log::println("Loop start");
  flasher.flash();

  // check queue for operation.
  // take next opertion step.
  body.action();

  Log::println("Loop end");
  delay(1000);
}
