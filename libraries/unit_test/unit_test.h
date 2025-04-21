#define SHOULD(x) if (!(x)) { Log::println("FAIL: %s", #x); } else { Log::println("PASS: %s", #x); }
#define SHOULD_NOT(x) if ((x)) { Log::println("FAIL: %s", #x); } else { Log::println("PASS: %s", #x); }


