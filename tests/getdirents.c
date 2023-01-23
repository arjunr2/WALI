#define _GNU_SOURCE
#include "common.h"

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)


#define BUF_SIZE 1024

const char* get_type(char d_type) {
  switch (d_type) {
    case DT_REG:   return "regular";
    case DT_DIR:  return "directory";
    case DT_FIFO: return "FIFO";
    case DT_SOCK: return "socket";
    case DT_LNK:  return "symlink";
    case DT_BLK:  return "block dev";
    case DT_CHR:  return "char dev";
    default:
      return "???";
  }
}

int main()
{
    int fd, nread;
    char buf[BUF_SIZE];
    struct dirent *d;
    int bpos;
    char d_type;

    fd = open("../tools", O_RDONLY | O_DIRECTORY);
    if (fd == -1)
        handle_error("open");

    for ( ; ; ) {
        PRINT_INT("getdents", SYS_getdents);
        PRINT_INT("getdents64", SYS_getdents64);
        nread = syscall(SYS_getdents64, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");

        if (nread == 0)
            break;

        PRINT_INT("--- nread", nread); 
        for (bpos = 0; bpos < nread;) {
            d = (struct dirent *) (buf + bpos);
            PRINT_INT("Inode", d->d_ino);
            PRINT_STR("Filetype", (char*) get_type(d->d_type));
            PRINT_INT("D_reclen", d->d_reclen);
            PRINT_INT("D_off", d->d_off);
            PRINT_STR("D_name", d->d_name);
            print("\n");
            bpos += d->d_reclen;
        }
    }
    return 0;
}

