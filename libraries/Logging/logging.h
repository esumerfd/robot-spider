#ifndef LOGGING_H
#define LOGGING_H

class Log {
  public:
    /*
     * Monitor port settings:
     *  baudrate=9600
     *  bits=8
     *  dtr=on
     *  parity=none
     *  rts=on
     *  stop_bits=1
     */
    static void begin();
    static void println(const char* format, ...);
    static void print(const char* format, ...);
};

#endif
