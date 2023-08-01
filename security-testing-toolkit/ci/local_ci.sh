#!/bin/bash
# Local CI script for security testing

set -e

echo "=== Running Local CI Security Tests ==="

# Static analysis
echo "1. Static Analysis..."
cd static_analysis
./cppcheck.sh ../src
./clang_tidy.sh ../src
cd ..

# Dynamic analysis
echo "2. Dynamic Analysis..."
cd dynamic_analysis
./valgrind.sh ../build/test_program
cd ..

# Fuzzing
echo "3. Fuzzing..."
cd fuzzing/afl
./run_afl.sh &
FUZZ_PID=$!
sleep 30
kill $FUZZ_PID 2>/dev/null || true
cd ../..

# Generate report
echo "4. Generating Report..."
python3 reports/generate_report.py

echo "=== CI Tests Complete ==="

