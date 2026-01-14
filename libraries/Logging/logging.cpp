#include <logging.h>

#include <HardwareSerial.h>

// Static buffer shared by print and println (not on stack)
// Single-threaded Arduino environment makes this safe
static char logBuffer[256];

void Log::begin() {
  Serial.begin(9600);
}

void Log::println(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vsnprintf(logBuffer, sizeof(logBuffer), format, args);
  va_end(args);
  Serial.println(logBuffer);
}

void Log::print(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vsnprintf(logBuffer, sizeof(logBuffer), format, args);
  va_end(args);
  Serial.print(logBuffer);
}
