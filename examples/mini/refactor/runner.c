#include "wali_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

wali_shm_t _wali_shm = {0};

int main(int argc, char** argv) {
    const char* shm_path = getenv("WALI_TEST_SHM");
    if (!shm_path) {
        fprintf(stderr, "[runner] WALI_TEST_SHM not set\n");
        return 1;
    }

    int fd = open(shm_path, O_RDWR);
    if (fd < 0) {
        perror("[runner] open");
        return 1;
    }

    size_t shm_size = 64 * 1024;
    char* ptr = (char*)mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("[runner] mmap");
        close(fd);
        return 1;
    }
    
    // Close the FD so the test starts with a cleaner file descriptor table
    close(fd);

    _wali_shm.ptr = ptr;
    _wali_shm.size = shm_size;
    _wali_shm.offset = 0;

    // Run the test directly. 
    // The Python harness handles process isolation and crash detection.
    return wali_test_main(argc, argv);
}
