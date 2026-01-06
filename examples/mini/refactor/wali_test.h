#ifndef WALI_TEST_H
#define WALI_TEST_H

#include <stddef.h>
#include <stdint.h>

// The shared memory buffer
// We use a fixed size buffer (64KB)
#define WALI_TEST_BUF_SIZE (64 * 1024)

extern uint8_t* _wali_test_buf_ptr;
extern size_t _wali_test_offset;

// User must implement this
int wali_test_main(int argc, char** argv);

// Helper functions
static inline void wali_test_write(const void* data, size_t size) {
    if (_wali_test_offset + size > WALI_TEST_BUF_SIZE) {
        return;
    }
    
    uint8_t* dst = _wali_test_buf_ptr + _wali_test_offset;
    const uint8_t* src = (const uint8_t*)data;
    for (size_t i = 0; i < size; i++) {
        dst[i] = src[i];
    }
    _wali_test_offset += size;
}

static inline void wali_test_log(const char* msg) {
    size_t len = 0;
    while (msg[len]) len++;
    wali_test_write(msg, len);
    wali_test_write("\n", 1);
}

static inline void wali_test_log_int(long val) {
    char buf[32];
    char* p = buf + sizeof(buf) - 1;
    *p = '\0';
    
    long v = val;
    int neg = 0;
    if (v < 0) {
        neg = 1;
        v = -v;
    }
    
    if (v == 0) {
        *--p = '0';
    } else {
        while (v > 0) {
            *--p = (v % 10) + '0';
            v /= 10;
        }
    }
    if (neg) *--p = '-';
    
    size_t len = 0;
    char* s = p;
    while (*s++) len++;
    
    wali_test_write(p, len);
    wali_test_write("\n", 1);
}

#endif
