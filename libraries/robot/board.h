#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#define SERVOMIN 150
#define SERVOMAX 600

/*
 * Definition of the board we are running on.
 */
class Board {

  public:

    int pwmSDA();
    int pwmSCL();

    uint16_t servoMin();
    uint16_t servoMax();
    uint16_t servoMiddle();
};

#endif

