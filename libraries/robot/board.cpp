#include <board.h>

#define I2C_SDA 15
#define I2C_SCL 14

int Board::pwmSDA() {
  return I2C_SDA;
}

int Board::pwmSCL() {
  return I2C_SCL;
}

uint16_t Board::servoMin() {
  return SERVOMIN;
}

uint16_t Board::servoMax() {
  return SERVOMAX;
}

uint16_t Board::servoMiddle() {
  return SERVOMIN + (SERVOMAX - SERVOMIN / 2);
}
