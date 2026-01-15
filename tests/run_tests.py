import os
import subprocess
import sys
import glob
import argparse
import shlex
import json
import re

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
    explicit_runs = []

    if not os.path.exists(source_file):
        return [{'setup': [], 'args': [], 'env': {}}]
        
    with open(source_file, 'r') as f:
        for line in f:
            line = line.strip()
            # Support // CMD: setup=".." args=".." env=".." (or simply "arg" for both)
            if line.startswith("// CMD:"):
                parts = shlex.split(line[len("// CMD:"):].strip())
                cmd = {'setup': [], 'cleanup': [], 'args': [], 'env': {}}
                for p in parts:
                    if p.startswith("setup="):
                        cmd['setup'].extend(shlex.split(p.split("=", 1)[1]))
                    elif p.startswith("cleanup="):
                        cmd['cleanup'].extend(shlex.split(p.split("=", 1)[1]))
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
    
    # Add explicit runs
    final_runs.extend(explicit_runs)
    
    if not final_runs:
        return [{'setup': [], 'cleanup': [], 'args': [], 'env': {}}]
        
    return final_runs

def run_test_case_execution(base_name, run_config, engine, verbose, run_idx, num_runs):
    run_args_setup = run_config['setup']
    run_args_cleanup = run_config.get('cleanup', [])
    if not run_args_cleanup:
        run_args_cleanup = run_args_setup
        
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
            # hooks_bin cleanup [cleanup_args...]
            cleanup_cmd = [hooks_bin, "cleanup"] + run_args_cleanup
            subprocess.check_output(cleanup_cmd, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            cleanup_out = e.output.decode('utf-8', errors='replace')
            return False, (f"Native cleanup failed (exit code {e.returncode})", native_out + "\n[Cleanup Output]\n" + cleanup_out, "")
    
    # Read/Clean Native Results
    native_result_data = b""
    if os.path.exists(result_file):
        with open(result_file, "rb") as f:
            native_result_data = f.read()
        os.remove(result_file) 

    # --- Run Wasm ---
    
    # Generate Env File for WALI
    env_file_path = f"/tmp/wali_env_{base_name}_{run_idx}.env"
    with open(env_file_path, "w") as f:
        for k, v in run_env.items():
            f.write(f"{k}={v}\n")
            
    # Build Engine Command
    cmd_template = engine.get('command', "../iwasm")
    if isinstance(cmd_template, str):
        wasm_cmd_parts = [cmd_template]
    else:
        wasm_cmd_parts = list(cmd_template)

    arg_templates = engine.get('args', ["{verbose}", "--env-file={env_file}", "--native-lib={native_lib}", "{wasm_file}", "{args}"])
    verbose_arg = engine.get('verbose_arg', '') if verbose else ''
    
    final_args = []
    has_args_placeholder = False
    
    for arg in arg_templates:
        if "{args}" in arg:
            has_args_placeholder = True
            if arg == "{args}":
                final_args.extend(run_args_test)
        else:
             val = arg.replace("{verbose}", verbose_arg)
             val = val.replace("{env_file}", env_file_path)
             val = val.replace("{native_lib}", native_lib)
             val = val.replace("{wasm_file}", wasm_file)
             if val: 
                 final_args.append(val)
    
    if not has_args_placeholder:
        final_args.extend(run_args_test)
        
    iwasm_cmd = wasm_cmd_parts + final_args
    
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
             cleanup_cmd = [hooks_bin, "cleanup"] + run_args_cleanup
             subprocess.check_output(cleanup_cmd, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            cleanup_out = e.output.decode('utf-8', errors='replace')
            return False, (f"Wasm cleanup failed (exit code {e.returncode})", native_out, wasm_out + "\n[Cleanup Output]\n" + cleanup_out)
            
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

def run_test_suite(test_path, engines, verbose):
    base_name = os.path.basename(test_path).replace(".c", "")
    # Parse runs
    source_file = f"unit/{base_name}.c"
    runs = parse_runs(source_file)
    
    # Print Test Name
    print(f"{base_name:.<50}", end='', flush=True)

    suite_results = {}
    
    for engine in engines:
        failed_runs = []
        passed_count = 0
        for i, run_args in enumerate(runs):
            success, result_data = run_test_case_execution(base_name, run_args, engine, verbose, i, len(runs))
            if success:
                passed_count += 1
            else:
                failed_runs.append((i, run_args, result_data))
        suite_results[engine['name']] = (passed_count, failed_runs)

    # Check aggregate
    all_engines_passed = all(len(res[1]) == 0 for res in suite_results.values())
    
    if all_engines_passed:
        print(f"{GREEN}PASS ({len(runs)}/{len(runs)}){RESET}")
        return True
    else:
        print(f"{RED}FAIL{RESET}")
        for engine_name in suite_results:
            pc, failed = suite_results[engine_name]
            if len(failed) == 0:
                print(f"  [{engine_name}] PASS")
            else:
                print(f"  [{engine_name}] FAIL ({pc}/{len(runs)})")
                for idx, args, result_info in failed:
                    if isinstance(result_info, str):
                        print(f"    Run {idx+1} Failed: {args}")
                        print(f"      Reason: {result_info}")
                        continue
                    
                    reason, native_out, wasm_out = result_info
                    print(f"    Run {idx+1} Failed: {args}")
                    print(f"      Reason: {reason}")
                    print("      Native Output:")
                    for line in native_out.splitlines(): print(f"        {line}")
                    print("      Wasm Output:")
                    for line in wasm_out.splitlines(): print(f"        {line}")
        return False

def main():
    parser = argparse.ArgumentParser(description='Run WALI unit tests.')
    parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose output (iwasm -v=5 and full logs)')
    parser.add_argument('--config', default='engines.json', help='Path to engines config file')
    args = parser.parse_args()

    # Load Engines
    if not os.path.exists(args.config):
        print(f"Error: Config file {args.config} not found.")
        sys.exit(1)

    with open(args.config, 'r') as f:
        engines = json.load(f)

    print(f"Testing with engines: {', '.join(e['name'] for e in engines)}")
    print("-" * 60)

    # Find all .c files in unit/
    test_sources = glob.glob("unit/*.c")
    test_sources = [f for f in test_sources if not f.endswith("_hooks.c")]
    if not test_sources:
        print("No tests found in unit/")
        return
        
    tests = sorted([os.path.splitext(f)[0] for f in test_sources])

    success_count = 0
    for t in tests:
        if run_test_suite(t, engines, args.verbose):
            success_count += 1
            
    print(f"\nSummary: {success_count}/{len(tests)} test suites passed.")
    if success_count != len(tests):
        sys.exit(1)

if __name__ == "__main__":
    main()
