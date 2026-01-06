import os
import subprocess
import sys
import glob

def run_test(test_name):
    # test_name includes directory, e.g. "unit/test_basic"
    print(f"Running test: {test_name}")
    
    # Paths
    base_name = os.path.basename(test_name)
    wasm_file = f"bin/unit/wasm/{base_name}.wasm"
    native_file = f"bin/unit/elf/{base_name}"
    native_lib = "./libnative_lib.so"
    
    if not os.path.exists(wasm_file) or not os.path.exists(native_file):
        print(f"Skipping {test_name}: missing binary")
        return False

    # Run Native
    print("  Native execution:")
    try:
        native_out = subprocess.check_output([native_file], stderr=subprocess.STDOUT).decode('utf-8')
        print(native_out)
    except subprocess.CalledProcessError as e:
        print(f"  Native failed with return code {e.returncode}")
        native_out = e.output.decode('utf-8')
        print(native_out)

    # Run Wasm
    print("  Wasm execution:")
    # Assuming iwasm is in path or provided
    iwasm_cmd = ["../iwasm", "-v=5", "--native-lib=" + native_lib, wasm_file]
    try:
        print(f"    Command: {' '.join(iwasm_cmd)}")
        wasm_out = subprocess.check_output(iwasm_cmd, stderr=subprocess.STDOUT).decode('utf-8')
        print(wasm_out)
    except subprocess.CalledProcessError as e:
        print(f"  Wasm failed with return code {e.returncode}")
        wasm_out = e.output.decode('utf-8')
        print(wasm_out)

    # Compare
    # We might need to normalize output if there are slight differences (e.g. pointers)
    # For now, strict comparison or check for key phrases.
    # The native helper prints "[Native] ...", the wasm helper prints "[Host] ..."
    # So exact match won't work.
    # We should probably check that the "Test Basic: ..." lines match.
    
    native_lines = [l for l in native_out.splitlines() if l.startswith("Test")]
    wasm_lines = [l for l in wasm_out.splitlines() if l.startswith("Test")]
    
    if native_lines == wasm_lines:
        print("  SUCCESS: Outputs match")
        return True
    else:
        print("  FAILURE: Outputs differ")
        print("  Native relevant lines:")
        for l in native_lines: print(f"    {l}")
        print("  Wasm relevant lines:")
        for l in wasm_lines: print(f"    {l}")
        return False

def main():
    # Find all .c files in unit/
    test_sources = glob.glob("unit/*.c")
    # Store complete relative path without extension, e.g. "unit/test_basic"
    tests = [os.path.splitext(f)[0] for f in test_sources]
    
    if not tests:
        print("No tests found.")
        return

    success_count = 0
    for t in tests:
        if run_test(t):
            success_count += 1
            
    print(f"\nSummary: {success_count}/{len(tests)} tests passed.")
    if success_count != len(tests):
        sys.exit(1)

if __name__ == "__main__":
    main()
