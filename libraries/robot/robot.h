#ifndef ROBOT_H
#define ROBOT_H

#include <vector>
#include <flasher.h>
#include <board.h>
#include <body.h>
#include <one_sweep_sequence.h>
#include <multi_step_gait.h>
#include <gait_sequences.h>
#include <command_router.h>
#include <bluetooth_connection.h>
#include <profiler.h>
#include <test_harness.h>

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
    MemoryProfiler _memoryProfiler;

    // Test harness for movement testing
    TestHarness _testHarness;

    uint32_t _lastUpdateMs;
    bool _firstLoop;

    // Command state
    bool _isMoving;
    String _currentCommand;

    // Command argument type alias
    using Args = const std::vector<String>&;

    // Command handlers (all receive arguments, even if unused)
    void handleInitCommand(Args args);
    void handleResetCommand(Args args);
    void handleForwardCommand(Args args);
    void handleBackwardCommand(Args args);
    void handleLeftCommand(Args args);
    void handleRightCommand(Args args);
    void handleStopCommand(Args args);
    void handleWiggleCommand(Args args);
    void handleTestMovementCommand(Args args);

    // Communication setup
    void setupCommands();

  public:
    Robot();

    void setup();
    void loop();
};

#endif
