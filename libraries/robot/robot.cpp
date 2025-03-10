#include <robot.h>
#include <flasher.h>
#include <logging.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define I2C_SDA 15
#define I2C_SCL 14

Flasher flasher;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

#define SERVOMIN 150
#define SERVOMAX 600

uint8_t servonum = 0;

void Robot::setup() {
  Log::begin();
  flasher.begin(1000);

  // seed initial sequence with movements.

  // init pwm
  Log::println("PWM Init");

  Wire.begin(I2C_SDA, I2C_SCL);

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
