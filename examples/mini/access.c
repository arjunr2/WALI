#include "common.h"
#include <errno.h>
#define SZ 100

int main(int argc, char* argv[]) {
  char path[200];
  if (argc > 1) {
    strcpy(path, argv[1]);
  } else {
    strcpy(path, "compile-wali.sh");
  }
  if (!access(path, F_OK)) {
    printf("OK access rights -- Errno: %s\n", strerror(errno));
    int fd =  open(path, O_RDONLY);
    lseek(fd, 156, SEEK_SET);
    char buf[SZ] = {0};
    ssize_t read_ct = read(fd, buf, SZ-1);
    PRINT_INT("Read ct", read_ct);
    for (int i = 0; i < SZ; i++) {
      print_char(buf[i]);
    }
    print("\n");
    close(fd);
  }
  else {
    printf("No access rights -- Errno: %s\n", strerror(errno));
  }
  return 0;
}
