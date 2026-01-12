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
    _profiler(false), // Profiling disabled by default
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

  delay(500);
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
  delay(100); // Give serial time to flush

  // Apply stationary gait - robot starts at rest
  _body.applyGait(_stationaryGait);
  _currentCommand = "stationary";
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
  _profiler.update(currentMs);

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
          _forwardGait.advance();
          _body.applyGait(_forwardGait);
        } else {
          _currentCommand = "stationary";
          _body.applyGait(_stationaryGait);
          _isMoving = false;
        }
      } else if (_currentCommand == "backward") {
        if (!_backwardGait.isComplete()) {
          _backwardGait.advance();
          _body.applyGait(_backwardGait);
        } else {
          _currentCommand = "stationary";
          _body.applyGait(_stationaryGait);
          _isMoving = false;
        }
      } else if (_currentCommand == "left") {
        if (!_leftGait.isComplete()) {
          _leftGait.advance();
          _body.applyGait(_leftGait);
        } else {
          _currentCommand = "stationary";
          _body.applyGait(_stationaryGait);
          _isMoving = false;
        }
      } else if (_currentCommand == "right") {
        if (!_rightGait.isComplete()) {
          _rightGait.advance();
          _body.applyGait(_rightGait);
        } else {
          _currentCommand = "stationary";
          _body.applyGait(_stationaryGait);
          _isMoving = false;
        }
      }
    }
  }

  delay(10); // Small delay for stability, actual timing handled by deltaMs
}

void Robot::setupCommands() {
  Log::println("Robot: Setting up command handlers");

  // Register command handlers with the router
  _commandRouter.registerCommand("init", [this]() { handleInitCommand(); });
  _commandRouter.registerCommand("reset", [this]() { handleResetCommand(); });
  _commandRouter.registerCommand("forward", [this]() { handleForwardCommand(); });
  _commandRouter.registerCommand("backward", [this]() { handleBackwardCommand(); });
  _commandRouter.registerCommand("left", [this]() { handleLeftCommand(); });
  _commandRouter.registerCommand("right", [this]() { handleRightCommand(); });
  _commandRouter.registerCommand("stop", [this]() { handleStopCommand(); });

  // Hook up Bluetooth message callback to command router
  _bluetooth.onMessageReceived([this](String message) {
    _commandRouter.route(message);
  });

  Log::println("Robot: Registered %d commands", _commandRouter.getCommandCount());
}

void Robot::handleInitCommand() {
  Log::println("Robot: Executing INIT command");
  _isMoving = false;
  _currentCommand = "";
  // Could reset robot to home position here
  _bluetooth.send("OK: Initialized");
}

void Robot::handleResetCommand() {
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

void Robot::handleForwardCommand() {
  Log::println("Robot: Executing FORWARD command");
  _currentCommand = "forward";
  _isMoving = true;

  _forwardGait.reset();  // Reset to step 0
  _body.applyGait(_forwardGait);

  _bluetooth.send("OK: Moving forward");
}

void Robot::handleBackwardCommand() {
  Log::println("Robot: Executing BACKWARD command");
  _currentCommand = "backward";
  _isMoving = true;

  _backwardGait.reset();  // Reset to step 0
  _body.applyGait(_backwardGait);

  _bluetooth.send("OK: Moving backward");
}

void Robot::handleLeftCommand() {
  Log::println("Robot: Executing LEFT command");
  _currentCommand = "left";
  _isMoving = true;

  _leftGait.reset();  // Reset to step 0
  _body.applyGait(_leftGait);

  _bluetooth.send("OK: Turning left");
}

void Robot::handleRightCommand() {
  Log::println("Robot: Executing RIGHT command");
  _currentCommand = "right";
  _isMoving = true;

  _rightGait.reset();  // Reset to step 0
  _body.applyGait(_rightGait);

  _bluetooth.send("OK: Turning right");
}

void Robot::handleStopCommand() {
  Log::println("Robot: Executing STOP command");
  _isMoving = false;
  _currentCommand = "";
  // Movement will naturally stop since _isMoving is false
  _bluetooth.send("OK: Stopped");
}
