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
};

#endif
