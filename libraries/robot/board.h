#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#define SERVOMIN 150
#define SERVOMAX 545

/*
 * Definition of the board we are running on.
 */
class Board {

  private:
    // Per-servo calibration offsets in degrees
    // Positive offset = servo rotates counter-clockwise
    // Negative offset = servo rotates clockwise
    static const int8_t SERVO_CALIBRATION_OFFSETS[12];

  public:

    int pwmSDA();
    int pwmSCL();

    // Angle-based interface (0-180 degrees)
    float servoMin();
    float servoMax();
    float servoRange();
    float servoSpeed();  // Default constant speed
    float servoSpeed(uint16_t durationMs, float distance);  // Calculate speed based on duration
    float servoMiddle();
    float servoSafeMin() const { return 2.0f; }
    float servoSafeMax() const { return 178.0f; }

    // Calibration and conversion
    int8_t getServoCalibrationOffset(uint8_t servoNum) const;
    uint16_t angleToPWM(uint8_t servoNum, float angle) const;
    float pwmToAngle(uint8_t servoNum, uint16_t pwm) const;
};

#endif

