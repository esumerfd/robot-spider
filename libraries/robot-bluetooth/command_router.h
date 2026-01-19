#ifndef COMMAND_ROUTER_H
#define COMMAND_ROUTER_H

#include <Arduino.h>
#include <functional>
#include <map>
#include <vector>

/**
 * CommandRouter - Routes incoming string commands to registered handler functions
 *
 * This class provides a central dispatch mechanism for commands received from
 * various communication interfaces (Bluetooth, WebSocket, etc.). It maps command
 * strings to callback functions, enabling a clean separation between communication
 * layers and command execution logic.
 *
 * Commands are parsed as: "command arg1 arg2, arg3" where:
 * - First word is the command name
 * - Subsequent words (separated by spaces and/or commas) are arguments
 *
 * Supported commands (matching Android app interface):
 * - "init" - Initialize robot state
 * - "forward" - Move forward
 * - "backward" - Move backward
 * - "left" - Turn left
 * - "right" - Turn right
 * - "wiggle <servo>" - Test servo connectivity
 */
class CommandRouter {
  public:
    // Command handler function type - receives list of arguments
    using CommandHandler = std::function<void(const std::vector<String>&)>;

    /**
     * Register a handler for a specific command string
     *
     * @param command The command string (e.g., "forward")
     * @param handler The function to call when this command is received
     */
    void registerCommand(const String& command, CommandHandler handler);

    /**
     * Route an incoming message to the appropriate handler
     *
     * Parses the message, looks up the command, and invokes the registered handler.
     * If no handler is registered for the command, logs a warning.
     *
     * @param message The command message string (may include newlines)
     * @return true if command was recognized and handled, false otherwise
     */
    bool route(const String& message);

    /**
     * Check if a command handler is registered
     *
     * @param command The command string to check
     * @return true if a handler is registered for this command
     */
    bool hasCommand(const String& command) const;

    /**
     * Get count of registered commands
     *
     * @return Number of registered command handlers
     */
    size_t getCommandCount() const;

  private:
    std::map<String, CommandHandler> _handlers;

    /**
     * Parse message into command and arguments
     *
     * @param message Raw message string
     * @param outCommand Output: the command name (first word, lowercase)
     * @param outArgs Output: vector of arguments (subsequent words)
     */
    void parseMessage(const String& message, String& outCommand, std::vector<String>& outArgs) const;
};

#endif
