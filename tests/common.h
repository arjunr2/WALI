#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/sysmacros.h>
#include <signal.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <utime.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>

int print(const char* buf) {
  int len = strlen(buf);
  write(1, buf, len);
  return len;
}

int print_int(int64_t val) {
  char res[20];
  int len = 0;
  if (val < 0) { val = (~val) + 1; };
  do {
    res[len++] = (val % 10) + '0';
    val /= 10;
  } while (val > 0);
  res[len] = 0;

  for (int j = 0; j < len/2; j++) {
    char k = res[j];
    res[j] = res[len-1-j];
    res[len-1-j] = k;
  }

  write(1, res, len);
  return len;
}

int print_char(char v) {
  write(1, &v, 1);
  return 0;
}

#define PRINT_INT(hd, int_val) {  \
  print(hd ":    ");  \
  print_int(int_val); \
  print("\n");  \
}

#define PRINT_STR(hd, str_val) {  \
  print(hd ":    ");  \
  print(str_val); \
  print("\n");  \
} 

#define PRINT_CHAR(hd, char_val) {  \
  print(hd ":    ");  \
  print_char(char_val); \
}

