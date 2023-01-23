#include "common.h"
#define SZ 100

int main() {
  char path[] = "compile-wali.sh";
  if (!access(path, F_OK)) {
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
    print("No access rights\n");
  }
  return 0;
}
