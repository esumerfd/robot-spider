#include <logging.h>

#include <HardwareSerial.h>

void Log::begin() {
  Serial.begin(9600);
  delay(100);
}

void Log::println(const char* format, ...) {
  char buffer[500];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  Serial.println(buffer);
}

void Log::print(const char* format, ...) {
  char buffer[500];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  Serial.print(buffer);
}
