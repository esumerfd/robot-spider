#include "command_router.h"
#include <logging.h>

void CommandRouter::registerCommand(const String& command, CommandHandler handler) {
  if (command.length() == 0) {
    Log::println("CommandRouter: Cannot register empty command");
    return;
  }

  if (_handlers.find(command) != _handlers.end()) {
    Log::println("CommandRouter: Warning - overwriting handler for '%s'", command.c_str());
  }

  _handlers[command] = handler;
  Log::println("CommandRouter: Registered command '%s'", command.c_str());
}

bool CommandRouter::route(const String& message) {
  if (message.length() == 0) {
    return false;
  }

  String command = parseCommand(message);

  if (command.length() == 0) {
    return false;
  }

  auto it = _handlers.find(command);
  if (it != _handlers.end()) {
    Log::println("CommandRouter: Routing command '%s'", command.c_str());
    it->second(); // Invoke the handler
    return true;
  } else {
    Log::println("CommandRouter: Unknown command '%s'", command.c_str());
    return false;
  }
}

bool CommandRouter::hasCommand(const String& command) const {
  return _handlers.find(command) != _handlers.end();
}

size_t CommandRouter::getCommandCount() const {
  return _handlers.size();
}

String CommandRouter::parseCommand(const String& message) const {
  String cmd = message;

  // Trim leading/trailing whitespace and newlines
  cmd.trim();

  // Convert to lowercase for case-insensitive matching
  cmd.toLowerCase();

  return cmd;
}
