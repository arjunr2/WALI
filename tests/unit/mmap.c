// CMD: args="anon"
// CMD: setup="/tmp/map_file.txt" args="file /tmp/map_file.txt" cleanup="/tmp/map_file.txt"

#include "wali_start.c"
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    if (argc >= 1 && strcmp(argv[0], "file") != 0 && strstr(argv[0], "map_file") != NULL) {
         // Argv[0] is path
         int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
         if (fd >= 0) {
            write(fd, "0123456789", 10);
            close(fd);
         }
    }
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc >= 1) {
        unlink(argv[0]);
    }
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_mmap")))
long long __imported_wali_mmap(void *addr, size_t length, int prot, int flags, int fd, long long offset);
__attribute__((__import_module__("wali"), __import_name__("SYS_munmap")))
long long __imported_wali_munmap(void *addr, size_t length);
// Use long for open/close to match other files and likely runtime signature (i32)
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
  long long res = __imported_wali_mmap(addr, length, prot, flags, fd, (long long)offset);
  // Check for error (negative) - but in Wasm addresses are u32. 
  // However, syscall returns s64? If it's a pointer, it usually returns i32 on wasm32?
  // WIT says `ptr-void` -> `s64` (syscall-result).
  // Native mmap returns void*.
  // If error, it returns -1 (MAP_FAILED).
  // We need to cast carefully.
  if (res < 0 && res > -4096) { // basic errno check range
      // errno = -res;
      return MAP_FAILED;
  }
  return (void *)(long)res;
}
int wali_munmap(void *addr, size_t length) {
  return (int)__imported_wali_munmap(addr, length);
}
#else
#include <sys/syscall.h>
void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
  return (void *)syscall(SYS_mmap, addr, length, prot, flags, fd, offset);
}
int wali_munmap(void *addr, size_t length) {
  return syscall(SYS_munmap, addr, length);
}
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *mode = (argc > 0) ? argv[0] : "fail";
  
  if (strcmp(mode, "anon") == 0) {
      size_t size = 4096;
      void *ptr = wali_mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      
      if (ptr == MAP_FAILED) return -1;
      
      // Write to it
      char *cp = (char *)ptr;
      cp[0] = 'A';
      cp[4095] = 'Z';
      
      if (cp[0] != 'A') return -1;
      
      if (wali_munmap(ptr, size) != 0) return -1;
      
      return 0;
  } else if (strcmp(mode, "file") == 0) {
      int fd = wali_open(argv[1], O_RDONLY, 0);
      if (fd < 0) return -1;
      
      // Map 10 bytes
      char *p = (char*)wali_mmap(NULL, 10, PROT_READ, MAP_PRIVATE, fd, 0);
      if (p == MAP_FAILED) { wali_close(fd); return -1; }
      
      if (p[0] != '0') { wali_munmap(p, 10); wali_close(fd); return -1; }
      if (p[9] != '9') { wali_munmap(p, 10); wali_close(fd); return -1; }
      
      if (wali_munmap(p, 10) != 0) { wali_close(fd); return -1; }
      wali_close(fd);
      return 0;
  }
  
  return -1;
}
