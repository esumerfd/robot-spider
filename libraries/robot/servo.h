#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <board.h>

/*
 * Simple abstraction of servo library
 */
class Servo {
  private:

    Board& _board;

    uint8_t _servonum = 0;
    uint16_t _position = SERVOMIN + (SERVOMAX - SERVOMIN) / 2;  // Start at middle (375)

    // PWM initialization flag - safe for single-threaded Arduino model
    // WARNING: Add synchronization if using FreeRTOS tasks or multi-core execution
    // See docs/adr/001-servo-pwm-initialization-thread-safety.md
    static bool _pwmInitialized;

  public:

    Servo(Board& board, uint8_t servonum);

    // Initialize PWM driver (call once before any servo operations)
    static void initializePWM(Board& board);

    void begin();
    void move(uint16_t position);
    uint16_t getPosition();
};

#endif
