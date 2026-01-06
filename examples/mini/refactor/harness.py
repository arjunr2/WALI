import os
import sys
import subprocess
import re
import tempfile
import shutil

# Configuration
# Assuming this script is in tests/refactor/
WALI_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../"))
TOOLCHAIN_SCRIPT = os.path.join(WALI_ROOT, "toolchains/wali.sh")
IWASM_BIN = os.path.join(WALI_ROOT, "iwasm")

def parse_test_config(c_file):
    config = {"args": "", "env": {}}
    with open(c_file, 'r') as f:
        content = f.read()
        match = re.search(r'RUN:\s*(.*)', content)
        if match:
            line = match.group(1)
            # Simple parsing for args="foo bar" env="A=B"
            args_match = re.search(r'args="(.*?)"', line)
            if args_match:
                config["args"] = args_match.group(1)
            
            env_match = re.findall(r'env="([^=]+)=([^"]+)"', line)
            for k, v in env_match:
                config["env"][k] = v
    return config

def compile_native(c_file, out_file):
    # Include the current directory for wali_test.h
    # Link with wali_lib.c
    runner_src = os.path.join(os.path.dirname(__file__), "wali_lib.c")
    cmd = ["gcc", runner_src, c_file, "-o", out_file, "-I" + os.path.dirname(c_file)]
    print(f"[Harness] Compiling Native: {' '.join(cmd)}")
    subprocess.check_call(cmd)

def compile_wasm(c_file, out_file):
    # We need to source the toolchain script to get environment variables
    # Then run clang.
    # Link with wali_lib.c
    
    runner_src = os.path.join(os.path.dirname(__file__), "wali_lib.c")
    
    # Note: We need to handle the output filename carefully.
    # The toolchain script sets WALI_CC.
    
    # We use the same flags as compile-wali-debug.sh roughly
    bash_cmd = f"""
    source {TOOLCHAIN_SCRIPT}
    
    $WALI_CC $WALI_COMMON_CFLAGS {runner_src} -c -o runner.o -I{os.path.dirname(c_file)}
    $WALI_CC $WALI_COMMON_CFLAGS {c_file} -c -o test.o -I{os.path.dirname(c_file)}
    
    $WALI_LD --no-gc-sections --no-entry --shared-memory --export-memory --max-memory=67108864 --undefined=__walirt_wasm_memory_size --allow-undefined -L$WALI_SYSROOT_DIR/lib runner.o test.o $WALI_SYSROOT_DIR/lib/crt1.o -lc -lm ${{WALI_LIBCLANG_RT_LIB:-}} -o {out_file}
    """
    
    env = os.environ.copy()
    env['cfile'] = c_file
    
    print(f"[Harness] Compiling WASM: {c_file} -> {out_file}")
    subprocess.check_call(["bash", "-c", bash_cmd], env=env)

def run_native(exe_file, shm_path, config):
    env = os.environ.copy()
    env.update(config["env"])
    env["WALI_TEST_SHM"] = shm_path
    
    args = [exe_file] + config["args"].split()
    print(f"[Harness] Running Native: {' '.join(args)}")
    subprocess.check_call(args, env=env)

def run_wasm(wasm_file, shm_path, config):
    env = os.environ.copy()
    env.update(config["env"])
    # Note: We don't set WALI_TEST_SHM in host env for iwasm to pick up automatically 
    # unless we pass it via --env.
    
    cmd = [IWASM_BIN, "--stack-size=4194304", "--max-threads=100"]
    
    # Create a temporary env file
    # We use a named temp file but we need to keep it around until subprocess finishes
    with tempfile.NamedTemporaryFile(mode='w', delete=False) as tmp_env:
        for k, v in config["env"].items():
            tmp_env.write(f"{k}={v}\n")
        tmp_env.write(f"WALI_TEST_SHM={shm_path}\n")
        tmp_env_path = tmp_env.name
        
    try:
        cmd.append(f"--env-file={tmp_env_path}")
        cmd.append(wasm_file)
        cmd.extend(config["args"].split())
        
        print(f"[Harness] Running WASM: {' '.join(cmd)}")
        subprocess.check_call(cmd, env=env)
    finally:
        if os.path.exists(tmp_env_path):
            os.remove(tmp_env_path)

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 harness.py <test_file.c>")
        sys.exit(1)
        
    c_file = os.path.abspath(sys.argv[1])
    base_name = os.path.splitext(os.path.basename(c_file))[0]
    build_dir = os.path.join(os.path.dirname(c_file), "build")
    os.makedirs(build_dir, exist_ok=True)
    
    native_exe = os.path.join(build_dir, f"{base_name}.elf")
    wasm_exe = os.path.join(build_dir, f"{base_name}.wasm")
    
    config = parse_test_config(c_file)
    
    # Compile
    try:
        compile_native(c_file, native_exe)
        compile_wasm(c_file, wasm_exe)
    except subprocess.CalledProcessError as e:
        print("[Harness] Compilation failed")
        sys.exit(1)
        
    # Create SHM files
    native_shm = os.path.join(build_dir, "native.shm")
    wasm_shm = os.path.join(build_dir, "wasm.shm")
    
    # Initialize files with zeros
    shm_size = 64 * 1024
    with open(native_shm, "wb") as f:
        f.write(b'\0' * shm_size)
    with open(wasm_shm, "wb") as f:
        f.write(b'\0' * shm_size)
        
    # Run
    try:
        run_native(native_exe, native_shm, config)
        run_wasm(wasm_exe, wasm_shm, config)
    except subprocess.CalledProcessError as e:
        print("[Harness] Execution failed")
        sys.exit(1)
        
    # Compare
    with open(native_shm, "rb") as f:
        native_data = f.read()
    with open(wasm_shm, "rb") as f:
        wasm_data = f.read()
        
    if native_data == wasm_data:
        print("[Harness] SUCCESS: Output matches")
    else:
        print("[Harness] FAILURE: Output mismatch")
        # Find first diff
        for i in range(shm_size):
            if native_data[i] != wasm_data[i]:
                print(f"Difference at byte {i}: Native={native_data[i]:02x}, WASM={wasm_data[i]:02x}")
                # Print context
                start = max(0, i - 10)
                end = min(shm_size, i + 10)
                print(f"Native context: {native_data[start:end]}")
                print(f"WASM context:   {wasm_data[start:end]}")
                break
        sys.exit(1)

if __name__ == "__main__":
    main()
