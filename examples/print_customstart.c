// Compile with `-nostartfiles` and `-Wl,--no-entry` to stub out main/start files
//
// Usually musl automatically includes the __init and __deinit startups, but here
// we bypass musl's initialization and define custom methods for these.

#include "stdio.h"

// Custom exported startup routine that calls wali.__init
__attribute__((export_name("custom_wali_startup")))
int startup() {
  // Engine initialization routine.
  // Must be called before any WALI-based routine is executed
  int __wali_init(void) __attribute((
    __import_module__("wali"),
    __import_name__("__init")
  ));

  return __wali_init();
}

// Custom exported startup routine that calls wali.__deinit
__attribute__((export_name("custom_wali_cleanup")))
int cleanup() {
  // Engine deinitialization routine
  // Must be called 
  int __wali_deinit(void) __attribute((
    __import_module__("wali"),
    __import_name__("__deinit")
  ));

  return __wali_deinit();
}



// We can call this method after `custom_wali_startup` to run
// some WALI code
__attribute__((export_name("print")))
int print() {
  // "write" is just a passthrough that doesn't use any internal musl state
  return write(1, "Hello world: 42\n", 16);
}
