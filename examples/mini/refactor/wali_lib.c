#include "wali_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// Define the buffer and offset
// Align to page size (64KB for WASM safety, though 4KB is typical for Linux)
uint8_t _wali_test_buf[WALI_TEST_BUF_SIZE] __attribute__((aligned(65536)));
uint8_t* _wali_test_buf_ptr = _wali_test_buf;
size_t _wali_test_offset = 0;

// Constructor to initialize the shared memory mapping
// This runs before main()
void wali_test_init() {
    const char* shm_path = getenv("WALI_TEST_SHM");
    if (!shm_path) {
        // If not set, we just use the local buffer (no sharing)
        // This allows running tests without the harness for debugging
        // printf("wali_test_init: WALI_TEST_SHM not set\n");
        return;
    }

    int fd = open(shm_path, O_RDWR);
    if (fd < 0) {
        // We can't easily print to stderr if we want to be minimal, 
        // but for the runner it's okay.
        // In WASM, this might go to host stdout/stderr.
        // printf("wali_test_init: open failed\n");
        return;
    }

    // Map the file. We try MAP_FIXED but if it fails/returns different address,
    // we update the pointer.
    void* ptr = mmap(_wali_test_buf, WALI_TEST_BUF_SIZE, 
                     PROT_READ | PROT_WRITE, 
                     MAP_FIXED | MAP_SHARED, 
                     fd, 0);
                     
    if (ptr == MAP_FAILED) {
        // Try without MAP_FIXED
        ptr = mmap(NULL, WALI_TEST_BUF_SIZE, 
                     PROT_READ | PROT_WRITE, 
                     MAP_SHARED, 
                     fd, 0);
        if (ptr == MAP_FAILED) {
             printf("wali_test_init: mmap failed completely\n");
             close(fd);
             return;
        }
    }
    
    // Update the pointer to use the shared memory
    _wali_test_buf_ptr = (uint8_t*)ptr;
    
    close(fd);
}

// Entry point wrapper
int main(int argc, char** argv) {
    wali_test_init();
    return wali_test_main(argc, argv);
}
