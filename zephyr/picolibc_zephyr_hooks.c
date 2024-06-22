#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "files/prog_defs.h"

static int fds = 4;
typedef struct {
  unsigned char *buf;
  int len;
  int ctlen;
} fmap_t;
static fmap_t filemap[100] = {0};

/* Import function bases */
int zephyr_fputc(int c, FILE *stream) __attribute__ ((
  __import_module__("wazi"),
  __import_name__("SYS_zephyr_fputc")
));

static int
picolibc_putc(char c, FILE *file) {
  zephyr_fputc(c, file);
  return 0;
}

static FILE __stdout = FDEV_SETUP_STREAM(picolibc_putc, NULL, NULL, _FDEV_SETUP_WRITE);
FILE *const stdout = &__stdout;
FILE *const stderr = &__stdout;

/** Imported functions for picolibc **/

/* I/O and file descriptors */
ssize_t write (int fd, const char *buf, size_t nbyte) {
  for (int i = 0; i < nbyte; i++) {
    zephyr_fputc(buf[i], stdout);
  }
  return nbyte;
}

#define STRCMP_CHECK(path) \
  else if (strcmp(pathname, #path ".lua") == 0) {  \
    filemap[retfd].buf = path##_lua; \
    filemap[retfd].len = path##_lua_len; \
    filemap[retfd].ctlen = path##_lua_len; \
  }


int open (const char* pathname, int flags, ...) {
  int retfd = fds + 1;
  if (0) {}
  STRCMP_CHECK(hello)
  STRCMP_CHECK(my)
  STRCMP_CHECK(fact)
  STRCMP_CHECK(fact_simple)
  STRCMP_CHECK(format)
  STRCMP_CHECK(arg)
  else {
#ifdef ZEPHYR_STUB_DEBUG
    printf("Error with pathname: %s\n", pathname);
#endif
    return -1;
  }
  fds = retfd;

#ifdef ZEPHYR_STUB_DEBUG
  printf("Opening \'%s\' with fd[%d]\n", pathname, retfd);
#endif
  return retfd;
}

ssize_t read(int fd, void *buf, size_t nbyte) {
  ssize_t retbytes = 0;
  if (filemap[fd].buf == NULL) {
#ifdef ZEPHYR_STUB_DEBUG
    printf("Error with fd[%d]\n", fd);
#endif
    return -1;
  }
  /* Truncate at top */
  if (nbyte > filemap[fd].ctlen) {
    retbytes = filemap[fd].ctlen;
  } else {
    retbytes = nbyte;
  }
#ifdef ZEPHYR_STUB_DEBUG
  printf("Reading fd[%d] for %d bytes; Return %d\n", fd, nbyte, retbytes);
#endif
  memcpy(buf, filemap[fd].buf, retbytes);
  filemap[fd].ctlen -= retbytes;
  return retbytes;
}

int close (int fd) {
  filemap[fd].buf = NULL;
  return 0;
}

/* Hardcoded unused stubs for now */
int gettimeofday (int a, int b) {
  return 0xBEEF;
}

int setjmp(int a) {
  return 0;
}
