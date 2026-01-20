#include <robot.h>
#include <logging.h>
#include <arduino.h>

// Constructor with member initializer list (guarantees correct order)
Robot::Robot()
  : _flasher(),
    _board(),
    _body(_board),
    _sweep(),
    _stationaryGait(&STATIONARY_SEQUENCE),
    _forwardGait(&FORWARD_WALK_SEQUENCE),
    _backwardGait(&BACKWARD_SEQUENCE),
    _leftGait(&LEFT_SEQUENCE),
    _rightGait(&RIGHT_SEQUENCE),
    _commandRouter(),
    _bluetooth(),
    _memoryProfiler(false), // Profiling disabled by default
    _lastUpdateMs(0),
    _firstLoop(true),
    _isMoving(false),
    _currentCommand("") {
}

void Robot::setup() {
  Log::begin();

  // Memory diagnostics at startup
  Log::println("=== ESP32 Memory Diagnostics ===");
  Log::println("Free heap: %d bytes", ESP.getFreeHeap());
  Log::println("Heap size: %d bytes", ESP.getHeapSize());
  Log::println("Min free heap: %d bytes", ESP.getMinFreeHeap());

  _flasher.begin();

  yield(); // Yield to watchdog

  // Initialize Bluetooth communication
  if (_bluetooth.begin("RobotSpider")) {
    Log::println("Robot: Bluetooth initialized successfully");
  } else {
    Log::println("Robot: Bluetooth initialization failed");
  }
  yield(); // Yield to watchdog

  // Setup command routing
  setupCommands();
  yield(); // Yield to watchdog

  // DEBUG: Re-enabled - testing Body::begin() incrementally
  _body.begin();
  yield(); // Yield to watchdog

  _lastUpdateMs = millis();

  // Memory diagnostics after initialization
  Log::println("After init - Free heap: %d bytes", ESP.getFreeHeap());
  Log::println("Robot: setup complete");

  // Apply stationary gait - robot starts at rest
  _body.applyGait(_stationaryGait);
  _currentCommand = "stationary";

  // Small delay to let initialized devices stabalize.
  delay(100);
}

void Robot::loop() {
  // Yield to watchdog to prevent ESP32 reset
  yield();

  // Process incoming Bluetooth messages
  _bluetooth.update();

  // Calculate elapsed time since last update
  uint32_t currentMs = millis();
  uint32_t deltaMs = currentMs - _lastUpdateMs;
  _lastUpdateMs = currentMs;

  // Periodic diagnostics (if enabled)
  _memoryProfiler.update(currentMs);

  // Update gait profilers
  _forwardGait.updateProfiler(currentMs);
  _backwardGait.updateProfiler(currentMs);
  _leftGait.updateProfiler(currentMs);
  _rightGait.updateProfiler(currentMs);

  _flasher.flash(currentMs);

  // Limit deltaMs on first loop to prevent huge jumps
  if (deltaMs > 100) {
    deltaMs = 100; // Cap at 100ms to prevent issues
  }

  // Update all legs (time-based movement) only if actively moving
  if (_isMoving) {
    _body.update(deltaMs);

    // When target is reached, advance to next step and reapply gait
    if (_body.atTarget()) {
      if (_currentCommand == "sweep") {
        _sweep.toggleDirection();
        _body.applyGait(_sweep);
      } else if (_currentCommand == "forward") {
        if (!_forwardGait.isComplete()) {
          Log::println("Robot: Step %d complete, advancing to step %d",
                       _forwardGait.getCurrentStep(),
                       _forwardGait.getCurrentStep() + 1);
          yield();  // Yield before step transition
          _forwardGait.advance();
          yield();  // Yield after advance
          _body.applyGait(_forwardGait);
          yield();  // Yield after applying new gait
        } else {
          Log::println("Robot: Forward gait complete");
          _currentCommand = "stationary";
          _body.applyGait(_stationaryGait);
          _isMoving = false;
        }
      } else if (_currentCommand == "backward") {
        if (!_backwardGait.isComplete()) {
          yield();
          _backwardGait.advance();
          yield();
          _body.applyGait(_backwardGait);
          yield();
        } else {
          _currentCommand = "stationary";
          _body.applyGait(_stationaryGait);
          _isMoving = false;
        }
      } else if (_currentCommand == "left") {
        if (!_leftGait.isComplete()) {
          yield();
          _leftGait.advance();
          yield();
          _body.applyGait(_leftGait);
          yield();
        } else {
          _currentCommand = "stationary";
          _body.applyGait(_stationaryGait);
          _isMoving = false;
        }
      } else if (_currentCommand == "right") {
        if (!_rightGait.isComplete()) {
          yield();
          _rightGait.advance();
          yield();
          _body.applyGait(_rightGait);
          yield();
        } else {
          _currentCommand = "stationary";
          _body.applyGait(_stationaryGait);
          _isMoving = false;
        }
      }
    }
  }
}

void Robot::setupCommands() {
  Log::println("Robot: Setting up command handlers");

  // Register command handlers with the router
  // All handlers receive arguments (even if unused)
  _commandRouter.registerCommand("init", [this](Args args) { handleInitCommand(args); });
  _commandRouter.registerCommand("reset", [this](Args args) { handleResetCommand(args); });
  _commandRouter.registerCommand("forward", [this](Args args) { handleForwardCommand(args); });
  _commandRouter.registerCommand("backward", [this](Args args) { handleBackwardCommand(args); });
  _commandRouter.registerCommand("left", [this](Args args) { handleLeftCommand(args); });
  _commandRouter.registerCommand("right", [this](Args args) { handleRightCommand(args); });
  _commandRouter.registerCommand("stop", [this](Args args) { handleStopCommand(args); });

  // Wiggle command for testing individual servo connectivity
  // Usage: "wiggle <servoName>" e.g., "wiggle leftfrontshoulder"
  _commandRouter.registerCommand("wiggle", [this](Args args) { handleWiggleCommand(args); });

  // Hook up Bluetooth message callback to command router
  _bluetooth.onMessageReceived([this](String message) {
    _commandRouter.route(message);
  });

  Log::println("Robot: Registered %d commands", _commandRouter.getCommandCount());
}

void Robot::handleInitCommand(Args args) {
  Log::println("Robot: Executing INIT command");
  _isMoving = false;
  _currentCommand = "";
  // Could reset robot to home position here
  _bluetooth.send("OK: Initialized");
}

void Robot::handleResetCommand(Args args) {
  Log::println("Robot: Executing RESET command");

  // Reset all gaits to step 0
  _stationaryGait.reset();
  _forwardGait.reset();
  _backwardGait.reset();
  _leftGait.reset();
  _rightGait.reset();

  // Move all servos to middle position
  _body.resetToMiddle();

  // Apply stationary gait and enable movement so servos can reach middle
  _body.applyGait(_stationaryGait);
  _currentCommand = "stationary";
  _isMoving = true;

  _bluetooth.send("OK: Reset to middle position");
}

void Robot::handleForwardCommand(Args args) {
  Log::println("Robot: Executing FORWARD command");
  _currentCommand = "forward";
  _isMoving = true;

  _forwardGait.reset();  // Reset to step 0
  _body.applyGait(_forwardGait);

  _bluetooth.send("OK: Moving forward");
}

void Robot::handleBackwardCommand(Args args) {
  Log::println("Robot: Executing BACKWARD command");
  _currentCommand = "backward";
  _isMoving = true;

  _backwardGait.reset();  // Reset to step 0
  _body.applyGait(_backwardGait);

  _bluetooth.send("OK: Moving backward");
}

void Robot::handleLeftCommand(Args args) {
  Log::println("Robot: Executing LEFT command");
  _currentCommand = "left";
  _isMoving = true;

  _leftGait.reset();  // Reset to step 0
  _body.applyGait(_leftGait);

  _bluetooth.send("OK: Turning left");
}

void Robot::handleRightCommand(Args args) {
  Log::println("Robot: Executing RIGHT command");
  _currentCommand = "right";
  _isMoving = true;

  _rightGait.reset();  // Reset to step 0
  _body.applyGait(_rightGait);

  _bluetooth.send("OK: Turning right");
}

void Robot::handleStopCommand(Args args) {
  Log::println("Robot: Executing STOP command");
  _isMoving = false;
  _currentCommand = "";
  // Movement will naturally stop since _isMoving is false
  _bluetooth.send("OK: Stopped");
}

void Robot::handleWiggleCommand(Args args) {
  if (args.empty()) {
    Log::println("Robot: WIGGLE command missing servo name");
    _bluetooth.send("ERROR: Missing servo name. Usage: wiggle <servoName>");
    return;
  }

  const String& servoName = args[0];
  Log::println("Robot: Executing WIGGLE command for '%s'", servoName.c_str());
  _isMoving = false;  // Stop any current movement

  if (_body.wiggleServo(servoName)) {
    _bluetooth.send("OK: Wiggled " + servoName);
  } else {
    _bluetooth.send("ERROR: Unknown servo " + servoName);
  }
}
