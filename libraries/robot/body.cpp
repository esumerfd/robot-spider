#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <body.h>

uint8_t servonum = 0;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

void Body::begin() {
  
  Wire.begin(I2C_SDA, I2C_SCL);

  pwm.begin();
  pwm.setPWMFreq(60);
  pwm.setPWM(servonum, 0, SERVOMIN + (SERVOMAX - SERVOMIN / 2));
}

