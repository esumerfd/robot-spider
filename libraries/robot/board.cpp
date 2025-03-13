#include <board.h>

#define I2C_SDA 15
#define I2C_SCL 14

int Board::pwmSDA() {
  return I2C_SDA;
}

int Board::pwmSCL() {
  return I2C_SCL;
}

int Board::servoMin() {
  return SERVOMIN;
}

int Board::servoMax() {
  return SERVOMAX;
}

int Board::servoMiddle() {
  return SERVOMIN + (SERVOMAX - SERVOMIN / 2);
}
