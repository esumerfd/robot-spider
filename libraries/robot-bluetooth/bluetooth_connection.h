#ifndef BLUETOOTH_CONNECTION_H
#define BLUETOOTH_CONNECTION_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <functional>

/**
 * BluetoothConnection - Manages Bluetooth Classic (SPP) communication
 *
 * This class wraps the ESP32 BluetoothSerial library and provides a
 * callback-based interface for receiving commands. It handles connection
 * management, message reception, and provides hooks for command processing.
 *
 * Usage:
 *   BluetoothConnection bt;
 *   bt.begin("RobotSpider");
 *   bt.onMessageReceived([](String msg) {
 *     // Handle message
 *   });
 *   bt.update(); // Call regularly in loop()
 */
class BluetoothConnection {
  public:
    // Message received callback type
    using MessageCallback = std::function<void(String)>;

    /**
     * Constructor
     */
    BluetoothConnection();

    /**
     * Initialize Bluetooth with device name
     *
     * @param deviceName The name that will appear when pairing (e.g., "RobotSpider")
     * @return true if initialization successful, false otherwise
     */
    bool begin(const String& deviceName);

    /**
     * Initialize Bluetooth with device name and PIN
     *
     * @param deviceName The name that will appear when pairing
     * @param pin PIN code for pairing security (4 digits recommended)
     * @return true if initialization successful, false otherwise
     */
    bool begin(const String& deviceName, const String& pin);

    /**
     * Register callback for received messages
     *
     * @param callback Function to call when a complete message is received
     */
    void onMessageReceived(MessageCallback callback);

    /**
     * Update - call this regularly in loop() to process incoming data
     *
     * Reads available data from Bluetooth serial, accumulates it into messages,
     * and invokes the callback when a complete message (terminated by newline) is received.
     */
    void update();

    /**
     * Check if a client is connected
     *
     * @return true if Bluetooth client is connected
     */
    bool isConnected();

    /**
     * Get the current device name
     *
     * @return The Bluetooth device name
     */
    String getDeviceName() const;

    /**
     * Send a message to the connected client
     *
     * @param message Message to send
     * @return true if message was sent successfully
     */
    bool send(const String& message);

    /**
     * Disconnect current client
     */
    void disconnect();

    /**
     * Stop Bluetooth and release resources
     */
    void end();

  private:
    BluetoothSerial _serialBT;
    MessageCallback _messageCallback;
    String _deviceName;
    String _messageBuffer;
    bool _initialized;

    static const size_t MAX_MESSAGE_LENGTH = 256;

    /**
     * Process accumulated buffer and extract complete messages
     */
    void processBuffer();
};

#endif
