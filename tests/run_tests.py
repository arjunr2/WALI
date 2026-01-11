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

def parse_env_str(env_str, env_dict):
    """
    Parses a string like "KEY=VAL KEY2=VAL2" and updates env_dict.
    Uses shlex to handle quoted values.
    """
    try:
        parts = shlex.split(env_str)
        for part in parts:
            if '=' in part:
                k, v = part.split('=', 1)
                env_dict[k] = v
    except:
        pass

def parse_runs(source_file):
    setup_configs = []
    test_arg_configs = []
    explicit_runs = []

    if not os.path.exists(source_file):
        return [{'setup': [], 'args': [], 'env': {}}]
        
    with open(source_file, 'r') as f:
        for line in f:
            line = line.strip()
            # Support // SETUP: env=".." arg1 arg2
            if line.startswith("// SETUP:"):
                parts = shlex.split(line[len("// SETUP:"):].strip())
                config = {'args': [], 'env': {}}
                for p in parts:
                    if p.startswith("env="):
                        parse_env_str(p[4:], config['env'])
                    else:
                        config['args'].append(p)
                setup_configs.append(config)

            # Support // TEST_ARGS: env=".." arg1 arg2
            elif line.startswith("// TEST_ARGS:"):
                 parts = shlex.split(line[len("// TEST_ARGS:"):].strip())
                 config = {'args': [], 'env': {}}
                 for p in parts:
                    if p.startswith("env="):
                        parse_env_str(p[4:], config['env'])
                    else:
                        config['args'].append(p)
                 test_arg_configs.append(config)

            # Support // CMD: setup=".." args=".." env=".." (or simply "arg" for both)
            elif line.startswith("// CMD:"):
                parts = shlex.split(line[len("// CMD:"):].strip())
                cmd = {'setup': [], 'args': [], 'env': {}}
                for p in parts:
                    if p.startswith("setup="):
                        cmd['setup'].extend(shlex.split(p.split("=", 1)[1]))
                    elif p.startswith("args="):
                        cmd['args'].extend(shlex.split(p.split("=", 1)[1]))
                    elif p.startswith("env="):
                        parse_env_str(p.split("=", 1)[1], cmd['env'])
                    else:
                        cmd['setup'].append(p)
                        cmd['args'].append(p)
                explicit_runs.append(cmd)

    # Generate combinations
    final_runs = []
    
    # If we have pool configs, generate cartesian product
    if setup_configs or test_arg_configs:
        if not setup_configs: setup_configs = [{'args': [], 'env': {}}]
        if not test_arg_configs: test_arg_configs = [{'args': [], 'env': {}}]
        
        for s in setup_configs:
            for t in test_arg_configs:
                # Merge envs (test overrides setup)
                merged_env = s['env'].copy()
                merged_env.update(t['env'])
                final_runs.append({'setup': s['args'], 'args': t['args'], 'env': merged_env})
    
    # Add explicit runs
    final_runs.extend(explicit_runs)
    
    if not final_runs:
        return [{'setup': [], 'args': [], 'env': {}}]
        
    return final_runs

def run_test_case_execution(base_name, run_config, verbose, run_idx, num_runs):
    run_args_setup = run_config['setup']
    run_args_test = run_config['args']
    run_env = run_config.get('env', {})

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
    
    # Setup Native Env
    env = os.environ.copy()
    env["WALI_TEST_RESULT_FILE"] = result_file
    env.update(run_env)

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
        # hooks_bin setup [setup_args...] -- [cmd... test_args...]
        native_cmd = [hooks_bin, "setup"] + run_args_setup + ["--"] + native_cmd + run_args_test
    else:
        native_cmd = native_cmd + run_args_test

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
            # hooks_bin cleanup [setup_args...]
            cleanup_cmd = [hooks_bin, "cleanup"] + run_args_setup
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
    
    # Generate Env File for WALI
    env_file_path = f"/tmp/wali_env_{base_name}_{run_idx}.env"
    with open(env_file_path, "w") as f:
        for k, v in run_env.items():
            f.write(f"{k}={v}\n")
            
    iwasm_flags.append(f"--env-file={env_file_path}")
        
    iwasm_cmd = ["../iwasm"] + iwasm_flags + ["--native-lib=" + native_lib, wasm_file] + run_args_test
    
    if hooks_bin:
        iwasm_cmd = [hooks_bin, "setup"] + run_args_setup + ["--"] + iwasm_cmd

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
             cleanup_cmd = [hooks_bin, "cleanup"] + run_args_setup
             subprocess.check_call(cleanup_cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except:
            pass
            
    # Cleanup env file
    if os.path.exists(env_file_path):
        os.remove(env_file_path)

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
    passed_count = 0
    
    for i, run_args in enumerate(runs):
        success, result_data = run_test_case_execution(base_name, run_args, verbose, i, len(runs))
        if success:
            passed_count += 1
        else:
            all_passed = False
            failed_runs.append((i, run_args, result_data))
            
    if all_passed:
        print(f"{GREEN}PASS ({passed_count}/{len(runs)}){RESET}")
        return True
    else:
        print(f"{RED}FAIL ({passed_count}/{len(runs)}){RESET}")
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
