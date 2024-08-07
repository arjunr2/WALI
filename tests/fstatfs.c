#include "common.h"
#include <sys/vfs.h>

int main() {
  struct statfs sb;
  int fd = open("compile-wali.sh", O_RDONLY);
  if (fstatfs(fd, &sb)) {
    printf("Error\n");
    return 1;
  }
  printf("Size of statfs: %lu\n", sizeof(sb));

  printf("F-type: %lu\n", sb.f_type);
  printf("F-bsize: %lu\n", sb.f_bsize);
  printf("F-blocks: %lu\n", sb.f_blocks);
  printf("F-bfree: %lu\n", sb.f_bfree);
  printf("F-bavail: %lu\n", sb.f_bavail);

  printf("F-files: %lu\n", sb.f_files);
  printf("F-ffree: %lu\n", sb.f_ffree);
  printf("F-fsid: %u | %u\n", sb.f_fsid.__val[0], sb.f_fsid.__val[1]);
  printf("F-namelen: %lu\n", sb.f_namelen);
  printf("F-frsize: %lu\n", sb.f_frsize);
  printf("F-flags: %lu\n", sb.f_flags);

  return 0;
}
