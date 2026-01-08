#include <robot.h>
#include <logging.h>
#include <arduino.h>

// Constructor with member initializer list (guarantees correct order)
Robot::Robot()
  : _flasher(),
    _board(),
    _body(_board),
    _sweep(),
    _commandRouter(),
    _bluetooth(),
    _lastUpdateMs(0),
    _lastHeapCheckMs(0),
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

  // Don't apply initial gait - wait for commands
  // _body.applyGait(_sweep);
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

  // Periodic heap monitoring (every 5 seconds)
  if (currentMs - _lastHeapCheckMs >= 5000) {
    Log::println("Heap: %d bytes (min: %d)", ESP.getFreeHeap(), ESP.getMinFreeHeap());
    _lastHeapCheckMs = currentMs;
  }

  _flasher.flash(currentMs);

  // Limit deltaMs on first loop to prevent huge jumps
  if (deltaMs > 100) {
    deltaMs = 100; // Cap at 100ms to prevent issues
  }

  // Update all legs (time-based movement) only if actively moving
  if (_isMoving) {
    _body.update(deltaMs);

    // For sweep test: when target is reached, toggle direction and reapply
    if (_currentCommand == "sweep" && _body.atTarget()) {
      _sweep.toggleDirection();
      _body.applyGait(_sweep);
    }
  }

  delay(10); // Small delay for stability, actual timing handled by deltaMs
}

void Robot::setupCommands() {
  Log::println("Robot: Setting up command handlers");

  // Register command handlers with the router
  _commandRouter.registerCommand("init", [this]() { handleInitCommand(); });
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

void Robot::handleForwardCommand() {
  Log::println("Robot: Executing FORWARD command");
  _currentCommand = "forward";
  _isMoving = true;

  // TODO: Apply forward walking gait when implemented
  // For now, use the sweep sequence as a demonstration
  _body.applyGait(_sweep);

  _bluetooth.send("OK: Moving forward");
}

void Robot::handleBackwardCommand() {
  Log::println("Robot: Executing BACKWARD command");
  _currentCommand = "backward";
  _isMoving = true;

  // TODO: Apply backward walking gait when implemented
  // For now, use the sweep sequence as a demonstration
  _body.applyGait(_sweep);

  _bluetooth.send("OK: Moving backward");
}

void Robot::handleLeftCommand() {
  Log::println("Robot: Executing LEFT command");
  _currentCommand = "left";
  _isMoving = true;

  // TODO: Apply left turn gait when implemented
  // For now, use the sweep sequence as a demonstration
  _body.applyGait(_sweep);

  _bluetooth.send("OK: Turning left");
}

void Robot::handleRightCommand() {
  Log::println("Robot: Executing RIGHT command");
  _currentCommand = "right";
  _isMoving = true;

  // TODO: Apply right turn gait when implemented
  // For now, use the sweep sequence as a demonstration
  _body.applyGait(_sweep);

  _bluetooth.send("OK: Turning right");
}

void Robot::handleStopCommand() {
  Log::println("Robot: Executing STOP command");
  _isMoving = false;
  _currentCommand = "";
  // Movement will naturally stop since _isMoving is false
  _bluetooth.send("OK: Stopped");
}
