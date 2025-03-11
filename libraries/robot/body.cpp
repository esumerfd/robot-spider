#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <body.h>

uint8_t servonum = 0;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

void Body::begin() {
  
  Wire.begin(I2C_SDA, I2C_SCL);

  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);
}

void Body::reset() {

  for (int pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++){
    pwm.setPWM(servonum, 0, pulselen);
    delay(10);
  }

  for (int pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--){
    pwm.setPWM(servonum, 0, pulselen);
    delay(10);
  }

}

