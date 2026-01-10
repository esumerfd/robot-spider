#include "bluetooth_connection.h"
#include <logging.h>

BluetoothConnection::BluetoothConnection()
  : _serialBT(),
    _messageCallback(nullptr),
    _deviceName(""),
    _messageBuffer(""),
    _initialized(false),
    _wasConnected(false) {
}

bool BluetoothConnection::begin(const String& deviceName) {
  if (_initialized) {
    Log::println("BluetoothConnection: Already initialized");
    return true;
  }

  _deviceName = deviceName;

  if (!_serialBT.begin(deviceName)) {
    Log::println("BluetoothConnection: Failed to initialize Bluetooth with name '%s'", deviceName.c_str());
    return false;
  }

  _initialized = true;
  Log::println("BluetoothConnection: Started successfully as '%s'", deviceName.c_str());
  Log::println("BluetoothConnection: Waiting for client connection...");

  return true;
}

bool BluetoothConnection::begin(const String& deviceName, const String& pin) {
  if (_initialized) {
    Log::println("BluetoothConnection: Already initialized");
    return true;
  }

  _deviceName = deviceName;

  // Set PIN before calling begin()
  // BluetoothSerial::setPin requires pin length as second parameter
  _serialBT.setPin(pin.c_str(), pin.length());

  if (!_serialBT.begin(deviceName)) {
    Log::println("BluetoothConnection: Failed to initialize Bluetooth with name '%s' and PIN", deviceName.c_str());
    return false;
  }

  _initialized = true;
  Log::println("BluetoothConnection: Started successfully as '%s' with PIN protection", deviceName.c_str());
  Log::println("BluetoothConnection: Waiting for client connection...");

  return true;
}

void BluetoothConnection::onMessageReceived(MessageCallback callback) {
  _messageCallback = callback;
}

void BluetoothConnection::update() {
  if (!_initialized) {
    return;
  }

  // Check for connection state changes and log them
  checkConnectionState();

  // Read all available data
  while (_serialBT.available()) {
    char c = _serialBT.read();

    // Add to buffer
    _messageBuffer += c;

    // Check for buffer overflow
    if (_messageBuffer.length() >= MAX_MESSAGE_LENGTH) {
      Log::println("BluetoothConnection: Message buffer overflow, clearing");
      _messageBuffer = "";
      continue;
    }

    // Check for complete message (newline terminated)
    Log::println("BluetoothConnection: Read char '%c'", c);
    if (c == '\n' || c == '\r') {
      processBuffer();
    }
  }
}

bool BluetoothConnection::isConnected() {
  return _initialized && _serialBT.hasClient();
}

String BluetoothConnection::getDeviceName() const {
  return _deviceName;
}

bool BluetoothConnection::send(const String& message) {
  if (!_initialized) {
    Log::println("BluetoothConnection: Cannot send - not initialized");
    return false;
  }

  if (!isConnected()) {
    Log::println("BluetoothConnection: Cannot send - no client connected");
    return false;
  }

  _serialBT.println(message);
  return true;
}

void BluetoothConnection::disconnect() {
  if (_serialBT.hasClient()) {
    _serialBT.disconnect();
    Log::println("BluetoothConnection: Client disconnected");
  }
}

void BluetoothConnection::end() {
  if (_initialized) {
    _serialBT.end();
    _initialized = false;
    _messageBuffer = "";
    Log::println("BluetoothConnection: Stopped");
  }
}

void BluetoothConnection::processBuffer() {
  // Trim the buffer
  _messageBuffer.trim();

  // Invoke callback if we have a non-empty message and a callback is registered
  if (_messageBuffer.length() > 0 && _messageCallback) {
    Log::println("BluetoothConnection: Received message: '%s'", _messageBuffer.c_str());
    _messageCallback(_messageBuffer);
  }

  // Clear buffer for next message
  _messageBuffer = "";
}

void BluetoothConnection::checkConnectionState() {
  bool currentlyConnected = isConnected();

  // Detect connection state change
  if (currentlyConnected && !_wasConnected) {
    Log::println("BluetoothConnection: Client connected");
    _wasConnected = true;
  } else if (!currentlyConnected && _wasConnected) {
    Log::println("BluetoothConnection: Client disconnected");
    _wasConnected = false;
    // Clear any partial message on disconnect
    _messageBuffer = "";
  }
}
