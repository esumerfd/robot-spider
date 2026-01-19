#include "command_router.h"
#include <logging.h>

void CommandRouter::registerCommand(const String& command, CommandHandler handler) {
  if (command.length() == 0) {
    Log::println("CommandRouter: Cannot register empty command");
    return;
  }

  // Store command in lowercase for case-insensitive matching
  String lowerCommand = command;
  lowerCommand.toLowerCase();

  if (_handlers.find(lowerCommand) != _handlers.end()) {
    Log::println("CommandRouter: Warning - overwriting handler for '%s'", lowerCommand.c_str());
  }

  _handlers[lowerCommand] = handler;
  Log::println("CommandRouter: Registered command '%s'", lowerCommand.c_str());
}

bool CommandRouter::route(const String& message) {
  if (message.length() == 0) {
    return false;
  }

  String command;
  std::vector<String> args;
  parseMessage(message, command, args);

  if (command.length() == 0) {
    return false;
  }

  auto it = _handlers.find(command);
  if (it != _handlers.end()) {
    if (args.empty()) {
      Log::println("CommandRouter: Routing command '%s'", command.c_str());
    } else {
      Log::println("CommandRouter: Routing command '%s' with %d args", command.c_str(), args.size());
    }
    it->second(args); // Invoke the handler with arguments
    return true;
  } else {
    Log::println("CommandRouter: Unknown command '%s'", command.c_str());
    return false;
  }
}

bool CommandRouter::hasCommand(const String& command) const {
  String lowerCommand = command;
  lowerCommand.toLowerCase();
  return _handlers.find(lowerCommand) != _handlers.end();
}

size_t CommandRouter::getCommandCount() const {
  return _handlers.size();
}

void CommandRouter::parseMessage(const String& message, String& outCommand, std::vector<String>& outArgs) const {
  String msg = message;
  msg.trim();

  outCommand = "";
  outArgs.clear();

  if (msg.length() == 0) {
    return;
  }

  // Tokenize by spaces and commas
  int start = 0;
  bool firstToken = true;

  for (int i = 0; i <= (int)msg.length(); i++) {
    char c = (i < (int)msg.length()) ? msg.charAt(i) : ' ';

    if (c == ' ' || c == ',' || c == '\t') {
      if (i > start) {
        String token = msg.substring(start, i);
        token.trim();

        if (token.length() > 0) {
          if (firstToken) {
            outCommand = token;
            outCommand.toLowerCase();
            firstToken = false;
          } else {
            // Keep argument case as-is (servo names are lowercase anyway)
            token.toLowerCase();
            outArgs.push_back(token);
          }
        }
      }
      start = i + 1;
    }
  }
}
