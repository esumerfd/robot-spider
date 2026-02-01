#ifndef LOGGING_H
#define LOGGING_H

/*
 * ESP32 Cam
 * Monitor port settings:
 *  baudrate=9600
 *  bits=8
 *  dtr=on
 *  parity=none
 *  rts=on
 *  stop_bits=1
 */
class Log {
  public:
    static void begin();
    static void println(const char* format, ...);
    static void print(const char* format, ...);

    // Debug mode - conditional logging for verbose output
    static void debugln(const char* format, ...);
    static void debug(const char* format, ...);
    static void setDebug(bool enabled);
    static bool isDebugEnabled();

  private:
    static bool _debugEnabled;
};

#endif
