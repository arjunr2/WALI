import os
import subprocess
import sys
import glob
import argparse
import shlex

# Colors
GREEN = '\033[92m'
RED = '\033[91m'
RESET = '\033[0m'
BOLD = '\033[1m'

def parse_runs(source_file):
    runs = []
    if not os.path.exists(source_file):
        return [[]]
        
    with open(source_file, 'r') as f:
        for line in f:
            line = line.strip()
            # Support // RUN: args
            if line.startswith("// RUN:"):
                args_str = line[len("// RUN:"):].strip()
                runs.append(shlex.split(args_str))
            # Support /* RUN: args */
            elif line.startswith("/* RUN:") and line.endswith("*/"):
                 args_str = line[len("/* RUN:"): -len("*/")].strip()
                 runs.append(shlex.split(args_str))
                 
    if not runs:
        return [[]]
    return runs

def run_test_case_execution(base_name, run_args, verbose, run_idx, num_runs):
    # Paths
    wasm_file = f"bin/unit/wasm/{base_name}.wasm"
    native_file = f"bin/unit/elf/{base_name}"
    native_lib = "./libnative_lib.so"
    
    if not os.path.exists(wasm_file) or not os.path.exists(native_file):
        return False, "missing binary"

    # Result file logic
    result_file = f"/tmp/wali_test_{base_name}_{run_idx}.bin"
    if os.path.exists(result_file):
        os.remove(result_file)
    env = os.environ.copy()
    env["WALI_TEST_RESULT_FILE"] = result_file

    # Hooks
    hooks_bin = f"bin/unit/elf/{base_name}_hooks"
    if not os.path.exists(hooks_bin):
        hooks_bin = None
        
    # --- Run Native ---
    native_returncode = 0
    native_out = ""
    
    native_cmd = [native_file]
    
    # Prepend hooks setup
    if hooks_bin:
        # hooks_bin setup [run_args...] -- [cmd...]
        native_cmd = [hooks_bin, "setup"] + run_args + ["--"] + native_cmd
    
    try:
        native_out = subprocess.check_output(
            native_cmd, 
            stderr=subprocess.STDOUT, 
            env=env
        ).decode('utf-8')
    except subprocess.CalledProcessError as e:
        native_returncode = e.returncode
        native_out = e.output.decode('utf-8')

    # Hooks cleanup
    if hooks_bin:
        try:
            # hooks_bin cleanup [run_args...]
            cleanup_cmd = [hooks_bin, "cleanup"] + run_args
            subprocess.check_call(cleanup_cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except:
            pass
    
    # Read/Clean Native Results
    native_result_data = b""
    if os.path.exists(result_file):
        with open(result_file, "rb") as f:
            native_result_data = f.read()
        os.remove(result_file) 

    # --- Run Wasm ---
    iwasm_flags = []
    if verbose:
        iwasm_flags.append("-v=5")
        
    iwasm_cmd = ["../iwasm"] + iwasm_flags + ["--native-lib=" + native_lib, wasm_file]
    
    if hooks_bin:
        iwasm_cmd = [hooks_bin, "setup"] + run_args + ["--"] + iwasm_cmd

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

    if hooks_bin:
        try:
             cleanup_cmd = [hooks_bin, "cleanup"] + run_args
             subprocess.check_call(cleanup_cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except:
            pass

    # Read/Clean Wasm Results
    wasm_result_data = b""
    if os.path.exists(result_file):
        with open(result_file, "rb") as f:
            wasm_result_data = f.read()
        os.remove(result_file)

    # --- Compare ---
    failure_reason = ""
    
    if native_returncode != wasm_returncode:
        failure_reason = f"Return codes differ (Native: {native_returncode}, Wasm: {wasm_returncode})"
    elif native_result_data != wasm_result_data:
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

    if not failure_reason:
        return True, (native_out, wasm_out)
    else:
        return False, (failure_reason, native_out, wasm_out)

def run_test_suite(test_path, verbose):
    base_name = os.path.basename(test_path).replace(".c", "")
    # Parse runs
    source_file = f"unit/{base_name}.c"
    runs = parse_runs(source_file)
    
    # Print Test Name
    print(f"{base_name:.<50}", end='', flush=True)
    
    all_passed = True
    failed_runs = []
    
    for i, run_args in enumerate(runs):
        success, result_data = run_test_case_execution(base_name, run_args, verbose, i, len(runs))
        if not success:
            all_passed = False
            failed_runs.append((i, run_args, result_data))
            
    if all_passed:
        print(f"{GREEN}PASS{RESET}")
        return True
    else:
        print(f"{RED}FAIL{RESET}")
        for idx, args, result_info in failed_runs:
             if isinstance(result_info, str):
                 print(f"  Run {idx+1} Failed: {args}")
                 print(f"    Reason: {result_info}")
                 continue
                 
             reason, native_out, wasm_out = result_info
             print(f"  Run {idx+1} Failed: {args}")
             print(f"    Reason: {reason}")
             print("    Native Output:")
             for line in native_out.splitlines(): print(f"      {line}")
             print("    Wasm Output:")
             for line in wasm_out.splitlines(): print(f"      {line}")
        return False

def main():
    parser = argparse.ArgumentParser(description='Run WALI unit tests.')
    parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose output (iwasm -v=5 and full logs)')
    args = parser.parse_args()

    # Find all .c files in unit/
    test_sources = glob.glob("unit/*.c")
    test_sources = [f for f in test_sources if not f.endswith("_hooks.c")]
    if not test_sources:
        print("No tests found in unit/")
        return
        
    tests = sorted([os.path.splitext(f)[0] for f in test_sources])

    success_count = 0
    for t in tests:
        if run_test_suite(t, args.verbose):
            success_count += 1
            
    print(f"\nSummary: {success_count}/{len(tests)} test suites passed.")
    if success_count != len(tests):
        sys.exit(1)

if __name__ == "__main__":
    main()
