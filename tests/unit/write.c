// CMD: setup="clean /tmp/write_file.txt" args="file /tmp/write_file.txt"
// CMD: args="bad_fd"
// CMD: args="stdout"

#include "wali_start.c"
#include <fcntl.h>
#include <stddef.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int file_exists(const char *path) {
    return access(path, F_OK) == 0;
}

int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *mode = argv[0];
    const char *path = argv[1];
    
    if (strcmp(mode, "clean") == 0) {
        unlink(path);
    }
    return 0;
}

int test_cleanup(int argc, char **argv) {
    // If we tested writing to a file, verify content
    int result = 0;
    if (argc >= 2 && strcmp(argv[0], "clean") == 0) {
        const char *path = argv[1];
        if (!file_exists(path)) {
             fprintf(stderr, "[Cleanup] Error: File %s not created\n", path);
             return 1;
        }
        
        FILE *f = fopen(path, "r");
        if (!f) {
            perror("[Cleanup] fopen");
            return 1;
        }
        char buf[128];
        if (fgets(buf, sizeof(buf), f)) {
            if (strcmp(buf, "WALI_WRITE_TEST") != 0) {
                fprintf(stderr, "[Cleanup] Error: File content mismatch. Got: '%s'\n", buf);
                result = 1;
            }
        } else {
             fprintf(stderr, "[Cleanup] Error: File empty\n");
             result = 1;
        }
        fclose(f);
        unlink(path);
    }
    return result;
}
#endif

#ifdef __wasm__
// Import WALI syscalls
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);

__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);

__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_open(const char *pathname, int flags, int mode) {
  return (int) __imported_wali_open(pathname, flags, mode);
}
int wali_write(int fd, const void *buf, size_t count) {
  return (int) __imported_wali_write(fd, buf, count);
}
int wali_close(int fd) {
  return (int) __imported_wali_close(fd);
}
#else
// Native: use raw syscall
#include <unistd.h>
#include <sys/syscall.h>

int wali_open(const char *pathname, int flags, int mode) {
  return syscall(SYS_open, pathname, flags, mode);
}
int wali_write(int fd, const void *buf, size_t count) {
  return syscall(SYS_write, fd, buf, count);
}
int wali_close(int fd) {
  return syscall(SYS_close, fd);
}
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *mode = (argc > 0) ? argv[0] : "stdout";
  
  int fd = -1;
  int should_succeed = 1;
  
  if (strcmp(mode, "file") == 0) {
      if (argc < 2) return -1;
      fd = wali_open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd < 0) return -1;
      should_succeed = 1;
  } else if (strcmp(mode, "bad_fd") == 0) {
      fd = 9999;
      should_succeed = 0;
  } else if (strcmp(mode, "stdout") == 0) {
      fd = 1;
      should_succeed = 1;
  } else {
      return -1;
  }

  const char *msg = "WALI_WRITE_TEST";
  
  int res = wali_write(fd, msg, strlen(msg));
  
  if (strcmp(mode, "file") == 0) {
      wali_close(fd);
  }
  
  if (should_succeed) {
      return (res == strlen(msg)) ? 0 : -1;
  } else {
      return (res < 0) ? 0 : -1;
  }
}
