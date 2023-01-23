#include "common.h"

int main() {
  struct utsname buf;
  uname(&buf);
  PRINT_STR("Sysname", buf.sysname);
  PRINT_STR("Nodename", buf.nodename);
  PRINT_STR("Release", buf.release);
  PRINT_STR("Version", buf.version);
  PRINT_STR("Machine", buf.machine);
  return 0;
}
