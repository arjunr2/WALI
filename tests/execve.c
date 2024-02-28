#include "common.h"

int main() {
  pid_t cpid = fork();
  /* Child */
  if (cpid == 0) {
    printf("Child PID: %d\n", getpid());
  }
  /* Parent */
  else {
    printf("Parent PID: %d\n", getpid());
    char *const environ[] = { "PATH=/home/arjun/.wasmtime/bin:/home/arjun/.wasmer/bin:/home/arjun/Documents/classes/vm/progress/bin:/home/arjun/tools/wabt/bin:"
      "/home/arjun/tools/dafny-3.5.0-x64-ubuntu-16.04/dafny:/home/arjun/Documents/research/webassembly/wali/wasm-micro-runtime/wamr-compiler/build:"
      "/home/arjun/Documents/research/webassembly/wali/wasm-micro-runtime/product-mini/platforms/linux/build:/home/arjun/tools/llvm-project/build/bin:"
      "/home/arjun/Documents/::/home/arjun/.cargo/bin:/home/arjun/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:"
      "/usr/local/games:/snap/bin:/home/arjun/.dotnet/tools:/home/arjun/.wasmer/globals/wapm_packages/.bin" };
    char program[] = "/home/arjun/Documents/research/webassembly/wali/wasm-micro-runtime/product-mini/platforms/linux/build/iwasm";
    char * argv[] = {NULL, "/home/arjun/Documents/research/webassembly/wali/tests/wasm/stat_link.wasm", NULL};
    argv[0] = program;
    int ret = execve(program, argv, environ);
    printf("ERROR: Shouldn't return from execv\n");
  }
  return 0;
}
