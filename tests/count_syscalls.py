#!/usr/bin/env python3
import os
import re

def main():
    # Determine the directory containing this script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # The target directory is bin/unit/wasm relative to the script
    wasm_dir = os.path.join(script_dir, 'bin', 'unit', 'wasm')

    if not os.path.exists(wasm_dir):
        print(f"Error: Directory {wasm_dir} does not exist.")
        return

    results = []

    # Regex to match WALI SYS_ imports
    # Example: (import "wali" "SYS_access" (func $__imported_wali_access (type 2)))
    # We look for (import "wali" "SYS_
    import_pattern = re.compile(r'\(import\s+"wali"\s+"SYS_')

    for root, dirs, files in os.walk(wasm_dir):
        for file in files:
            if file.endswith('.wat'):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        matches = import_pattern.findall(content)
                        count = len(matches)
                        results.append((file, count))
                except Exception as e:
                    print(f"Error reading {file}: {e}")

    # Sort results: primary key is count (descending), secondary key is filename (ascending) for stability
    results.sort(key=lambda x: (-x[1], x[0]))

    for filename, count in results:
        print(f"{filename}: {count}")

if __name__ == "__main__":
    main()
