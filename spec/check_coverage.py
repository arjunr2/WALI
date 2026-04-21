from syscall_definitions import _syscall_list
import os
import glob

# 1. Get implemented syscalls
implemented_syscalls = [s.name for s in _syscall_list if s.implemented]
implemented_syscalls.sort()

# 2. Get existing tests
# We assume test file name matches syscall name exactly, or we have a mapping.
# For now, let's just look at filenames.
test_dir = "../tests/unit"
existing_tests = set()
for f in glob.glob(os.path.join(test_dir, "*.c")):
    basename = os.path.splitext(os.path.basename(f))[0]
    existing_tests.add(basename)

# 3. Check coverage
missing = []
complex_tests = existing_tests.copy()
for syscall in implemented_syscalls:
    # Direct match?
    if syscall in existing_tests:
        complex_tests.remove(syscall)
        continue
    
    # 64-bit variants often mapped to base name? 
    # e.g. ftruncate could be ftruncate.c covering ftruncate
    # (The list has "ftruncate" as implemented. "ftruncate64" is not likely distinct in the python list unless it's an alias)
    
    missing.append(syscall)

print(f"Total Implemented Syscalls: {len(implemented_syscalls)}")
print(f"Existing Test Files: {len(existing_tests)}")
print(f"Complex Tests: {len(complex_tests)}")
for c in complex_tests:
    print(f"  {c}")
print(f"Missing Coverage ({len(missing)}):")
for m in missing:
    print(f"  {m}")
