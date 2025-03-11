#ifndef BOARD_H
#define BOARD_H

/*
 * Definition of the board we are running on.
 */
class Board {

  public:

    int pwmSDA();
    int pwmSCL();

    int servoMin();
    int servoMax();
    int servoMiddle();
};

#endif

