#include <robot.h>
#include <flasher.h>

Flasher flasher;

void Robot::setup() {
  flasher.begin(1000);

  // seed initial sequence with movements.

  // init pwm
  // init servos to default position
  // report status of memory, devices.

}

void Robot::loop() {
  flasher.flash();

  // check queue for operation.
  // take next opertion step.

  delay(1000);
}
