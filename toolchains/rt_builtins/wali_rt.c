#include <stdint.h>

__attribute__((export_name("__wasm_memory_grow")))
int32_t __walirt_wasm_memory_grow(int32_t pages) {
  return __builtin_wasm_memory_grow(0, pages);
}

__attribute__((export_name("__wasm_memory_size")))
int32_t __walirt_wasm_memory_size(void) {
  return __builtin_wasm_memory_size(0);
}
