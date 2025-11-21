#ifndef ROBOT_H
#define ROBOT_H

#include <flasher.h>
#include <board.h>
#include <body.h>
#include <arc_test_sequence.h>

class Robot {
  private:
    Flasher _flasher;
    Board _board;
    Body _body;
    ArcTestSequence _arcTest;

    uint32_t _lastUpdateMs;
    uint32_t _lastHeapCheckMs;
    bool _firstLoop;

  public:
    Robot();

    void setup();
    void loop();
};

#endif
