import os
import subprocess
import sys
import glob
import argparse

# Colors
GREEN = '\033[92m'
RED = '\033[91m'
RESET = '\033[0m'
BOLD = '\033[1m'

def run_test(test_name, verbose):
    base_name = os.path.basename(test_name)
    print(f"{test_name:.<50}", end='', flush=True)
    
    # Paths
    wasm_file = f"bin/unit/wasm/{base_name}.wasm"
    native_file = f"bin/unit/elf/{base_name}"
    native_lib = "./libnative_lib.so"
    
    if not os.path.exists(wasm_file) or not os.path.exists(native_file):
        print(f"{RED}SKIPPED{RESET} (missing binary, ensure compilation)")
        return False

    # Result file logic
    result_file = f"/tmp/wali_test_{base_name}.bin"
    if os.path.exists(result_file):
        os.remove(result_file)
    env = os.environ.copy()
    env["WALI_TEST_RESULT_FILE"] = result_file

    # --- Run Native ---
    native_returncode = 0
    native_out = ""
    try:
        native_out = subprocess.check_output(
            [native_file], 
            stderr=subprocess.STDOUT, 
            env=env
        ).decode('utf-8')
    except subprocess.CalledProcessError as e:
        native_returncode = e.returncode
        native_out = e.output.decode('utf-8')
    
    # Read/Clean Native Results
    native_result_data = b""
    if os.path.exists(result_file):
        with open(result_file, "rb") as f:
            native_result_data = f.read()
        os.remove(result_file) # Clean up for Wasm run

    # --- Run Wasm ---
    iwasm_flags = []
    if verbose:
        iwasm_flags.append("-v=5")
        
    iwasm_cmd = ["../iwasm"] + iwasm_flags + ["--native-lib=" + native_lib, wasm_file]
    wasm_returncode = 0
    wasm_out = ""
    try:
        wasm_out = subprocess.check_output(
            iwasm_cmd, 
            stderr=subprocess.STDOUT, 
            env=env
        ).decode('utf-8')
    except subprocess.CalledProcessError as e:
        wasm_returncode = e.returncode
        wasm_out = e.output.decode('utf-8')

    # Read/Clean Wasm Results
    wasm_result_data = b""
    if os.path.exists(result_file):
        with open(result_file, "rb") as f:
            wasm_result_data = f.read()
        os.remove(result_file)

    # --- Compare ---
    failure_reason = ""
    
    # 1. Return codes
    if native_returncode != wasm_returncode:
        failure_reason = f"Return codes differ (Native: {native_returncode}, Wasm: {wasm_returncode})"
    
    # 2. Result buffer
    elif native_result_data != wasm_result_data:
        # Find first difference
        diff_idx = -1
        max_len = max(len(native_result_data), len(wasm_result_data))
        for i in range(max_len):
            b_nat = native_result_data[i] if i < len(native_result_data) else -1
            b_wasm = wasm_result_data[i] if i < len(wasm_result_data) else -1
            if b_nat != b_wasm:
                diff_idx = i
                break
        
        failure_reason = f"Result buffer differs at offset {diff_idx}.\n"
        failure_reason += f"    Native byte: {hex(native_result_data[diff_idx]) if diff_idx < len(native_result_data) else 'EOF'}\n"
        failure_reason += f"    Wasm byte:   {hex(wasm_result_data[diff_idx]) if diff_idx < len(wasm_result_data) else 'EOF'}"

    # --- Output ---
    if not failure_reason:
        print(f"{GREEN}PASS{RESET}")
        if verbose:
            print("  Native Output:")
            for line in native_out.splitlines(): print(f"    {line}")
            print("  Wasm Output:")
            for line in wasm_out.splitlines(): print(f"    {line}")
        return True
    else:
        print(f"{RED}FAIL{RESET}")
        print(f"  Reason: {failure_reason}")
        print("  Native Output:")
        for line in native_out.splitlines(): print(f"    {line}")
        print("  Wasm Output:")
        for line in wasm_out.splitlines(): print(f"    {line}")
        return False

def main():
    parser = argparse.ArgumentParser(description='Run WALI unit tests.')
    parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose output (iwasm -v=5 and full logs)')
    args = parser.parse_args()

    # Find all .c files in unit/
    test_sources = glob.glob("unit/*.c")
    if not test_sources:
        print("No tests found in unit/")
        return
        
    tests = sorted([os.path.splitext(f)[0] for f in test_sources])

    success_count = 0
    for t in tests:
        if run_test(t, args.verbose):
            success_count += 1
            
    print(f"\nSummary: {success_count}/{len(tests)} tests passed.")
    if success_count != len(tests):
        sys.exit(1)

if __name__ == "__main__":
    main()
