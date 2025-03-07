#include <robot.h>
#include <flasher.h>

Flasher flasher;

void Robot::setup() {
  flasher.begin(1000);
}

void Robot::loop() {
  flasher.flash();
  delay(1000);
}
