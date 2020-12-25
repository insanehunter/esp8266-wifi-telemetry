#pragma once

#define DEBUG_PRINT_ENABLED 1

#if DEBUG_PRINT_ENABLED

  void debug_print(const Printable &msg);
  void debug_print(const String &msg);
  void debug_print(const char *msg);

#else // DEBUG_PRINT_ENABLED

  #define debug_print(something)

#endif // DEBUG_PRINT_ENABLED
