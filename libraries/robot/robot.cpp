
#include <robot.h>
#include <flasher.h>
#include <logging.h>

#include <body.h>
#include <board.h>

Flasher flasher;

Board board;
Body body = Body(board);

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

  Log::println("Loop end");
  delay(1000);
}
