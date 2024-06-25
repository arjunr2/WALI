#include <stdio.h>
#include <string.h>

int zsock_gethostname(char *buf, size_t len) __attribute__ ((
  __import_module__("wazi"),
  __import_name__("SYS_zsock_gethostname")
));

#define N 100

int main() {
  char buf[N];
  if (zsock_gethostname(buf, N)) {
    printf("Error in gethostname call\n");
    return 0;
  }
  printf("Hostname: %s\n", buf);
  return 0;
}
