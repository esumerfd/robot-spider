#ifndef ROBOT_H
#define ROBOT_H

#include <flasher.h>
#include <board.h>
#include <body.h>
#include <one_sweep_sequence.h>
#include <multi_step_gait.h>
#include <gait_sequences.h>
#include <command_router.h>
#include <bluetooth_connection.h>
#include <profiler.h>

class Robot {
  private:
    Flasher _flasher;
    Board _board;
    Body _body;
    OneSweepSequence _sweep;
    MultiStepGait _stationaryGait;
    MultiStepGait _forwardGait;
    MultiStepGait _backwardGait;
    MultiStepGait _leftGait;
    MultiStepGait _rightGait;

    // Communication components
    CommandRouter _commandRouter;
    BluetoothConnection _bluetooth;

    // Diagnostics
    Profiler _profiler;

    uint32_t _lastUpdateMs;
    bool _firstLoop;

    // Command state
    bool _isMoving;
    String _currentCommand;

    // Command handlers
    void handleInitCommand();
    void handleResetCommand();
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
