#ifndef BOARD_H
#define BOARD_H

#define SERVOMIN 150
#define SERVOMAX 600

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

