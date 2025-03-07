#include <arduino.h>
#include <robot.h>

Robot robot;

void setup() {
  robot.setup();
}

void loop() {
  robot.loop();
}
