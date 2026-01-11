# Profiler Library

Runtime performance monitoring and diagnostics for ESP32.

## Overview

The Profiler class provides configurable periodic logging of memory and performance metrics. It's designed to be lightweight and can be enabled/disabled to minimize overhead in production builds.

## Features

- **Heap Memory Monitoring**: Tracks free heap and minimum free heap
- **Configurable Interval**: Set custom logging intervals
- **Runtime Control**: Enable/disable profiling at runtime
- **Zero Overhead When Disabled**: No performance impact when profiling is off
- **Simple Integration**: Single method call in loop()

## Usage

### Basic Usage (Disabled by Default)

```cpp
#include <profiler.h>

Profiler profiler; // Disabled by default

void setup() {
  Log::begin();
  // ... other setup code
}

void loop() {
  uint32_t currentMs = millis();
  profiler.update(currentMs); // Does nothing when disabled

  // ... rest of loop
}
```

### Enable Profiling

```cpp
// Enable at construction
Profiler profiler(true); // Logs every 5 seconds by default

// Or enable at runtime
profiler.setEnabled(true);
```

### Custom Interval

```cpp
// Log every 10 seconds
Profiler profiler(true, 10000);

// Or change at runtime
profiler.setInterval(10000);
```

### Example Output

When enabled, the profiler logs heap statistics periodically:

```
Heap: 284840 bytes (min: 275120)
Heap: 283920 bytes (min: 275120)
Heap: 284100 bytes (min: 274980)
```

## API Reference

### Constructor

```cpp
Profiler(bool enabled = false, uint32_t intervalMs = 5000)
```

- `enabled`: Enable profiling output (default: false)
- `intervalMs`: Logging interval in milliseconds (default: 5000)

### Methods

#### `void update(uint32_t currentMs)`

Call this in your loop() function. Checks if it's time to log and outputs heap statistics if enabled.

- `currentMs`: Current time from `millis()`

#### `void setEnabled(bool enabled)`

Enable or disable profiling at runtime.

- `enabled`: true to enable logging, false to disable

#### `bool isEnabled() const`

Check if profiling is currently enabled.

- Returns: true if enabled, false otherwise

#### `void setInterval(uint32_t intervalMs)`

Change the logging interval.

- `intervalMs`: Interval in milliseconds between log outputs

## Integration with Robot

The Robot class uses the Profiler with profiling disabled by default:

```cpp
class Robot {
  private:
    Profiler _profiler;
    // ...
};

Robot::Robot()
  : _profiler(false), // Disabled by default
    // ...
{
}

void Robot::loop() {
  uint32_t currentMs = millis();
  _profiler.update(currentMs);
  // ...
}
```

To enable profiling during debugging, change the constructor:

```cpp
Robot::Robot()
  : _profiler(true), // Enable profiling
    // ...
{
}
```

## Performance Considerations

- **Disabled**: Zero overhead - the update() method returns immediately
- **Enabled**: Minimal overhead - only checks time and logs at specified intervals
- **No floating point operations**: Uses integer arithmetic for efficiency
- **No dynamic memory allocation**: All state is stack-allocated

## Future Enhancements

Potential additions to the profiler:

- Task execution timing
- Loop cycle timing
- Custom metric logging
- Statistical analysis (min/max/average)
- Performance threshold alerts
- CSV export for analysis
