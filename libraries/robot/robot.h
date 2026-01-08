#ifndef ROBOT_H
#define ROBOT_H

#include <flasher.h>
#include <board.h>
#include <body.h>
#include <one_sweep_sequence.h>
#include <command_router.h>
#include <bluetooth_connection.h>

class Robot {
  private:
    Flasher _flasher;
    Board _board;
    Body _body;
    OneSweepSequence _sweep;

    // Communication components
    CommandRouter _commandRouter;
    BluetoothConnection _bluetooth;

    uint32_t _lastUpdateMs;
    uint32_t _lastHeapCheckMs;
    bool _firstLoop;

    // Command state
    bool _isMoving;
    String _currentCommand;

    // Command handlers
    void handleInitCommand();
    void handleForwardCommand();
    void handleBackwardCommand();
    void handleLeftCommand();
    void handleRightCommand();
    void handleStopCommand();

    // Communication setup
    void setupCommands();

  public:
    Robot();

    void setup();
    void loop();
};

#endif
