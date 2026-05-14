#!/usr/bin/env python3
import subprocess
import sys
import os

PROGRAM = "../csvreader"
INPUT_DIR = "input"
EXPECTED_DIR = "expected"

def run_test(test_name):
    input_path = os.path.join(INPUT_DIR, test_name + ".csv")
    expected_path = os.path.join(EXPECTED_DIR, test_name + ".csv")
    
    if not os.path.exists(input_path):
        print(f"Missing input: {input_path}")
        return False
    
    proc = subprocess.run([PROGRAM, input_path], capture_output=True, text=True)
    
    if os.path.exists(expected_path):
        with open(expected_path, 'r') as f:
            expected = f.read()
    else:
        expected = ""
    
    expected = expected.rstrip()
    actual = proc.stdout.rstrip()
    
    if expected == "" and proc.returncode != 0:
        print(f"{test_name}: PASS (error exit)")
        return True
    elif expected == "" and proc.returncode == 0:
        print(f"{test_name}: FAIL (expected error but got output)")
        return False
    elif actual == expected:
        print(f"{test_name}: PASS")
        return True
    else:
        print(f"{test_name}: FAIL")
        print("Expected:")
        print(expected)
        print("Got:")
        print(actual)
        return False

def main():
    if not os.path.exists(PROGRAM):
        print(f"Program {PROGRAM} not found. Compile first.")
        sys.exit(1)
    
    tests = [f[:-4] for f in os.listdir(INPUT_DIR) if f.endswith(".csv")]
    tests.sort()
    
    if not tests:
        print("No tests found in input/")
        sys.exit(1)
    
    passed = 0
    failed = 0
    for t in tests:
        if run_test(t):
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {passed} passed, {failed} failed")
    sys.exit(0 if failed == 0 else 1)

if __name__ == "__main__":
    main()
    