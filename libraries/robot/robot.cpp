#include <robot.h>
#include <flasher.h>
#include <logging.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Flasher flasher;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150
#define SERVOMAX 600

uint8_t servonum = 0;

void Robot::setup() {
  Log::begin();
  flasher.begin(1000);

  // seed initial sequence with movements.

  // init pwm
  Log::println("PWM Init");
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);

  // init servos to default position
  // report status of memory, devices.

}

void Robot::loop() {
  Log::println("Loop start");

  for (int pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++){
    pwm.setPWM(servonum, 0, pulselen);
    delay(10);
  }

  for (int pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--){
    pwm.setPWM(servonum, 0, pulselen);
    delay(10);
  }

  flasher.flash();

  // check queue for operation.
  // take next opertion step.

  Log::println("Loop end");
  delay(1000);
}
