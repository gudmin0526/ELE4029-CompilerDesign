#!/bin/bash

make cminus_semantic MAIN=test

# Make cimpl
echo "---MAKE PARSER---"
./cminus_semantic cases/test/test.1.txt cases/result/semantic/result.1.txt
echo "COMPILE 1 COMPLETE"
./cminus_semantic cases/test/test.2.txt cases/result/semantic/result.2.txt
echo "COMPILE 2 COMPLETE"
./cminus_semantic cases/test/test.3.txt cases/result/semantic/result.3.txt
echo "COMPILE 3 COMPLETE"
./cminus_semantic cases/test/test.4.txt cases/result/semantic/result.4.txt
echo "COMPILE 4 COMPLETE"

echo "[TEST 1]"
cases/diff_checker cases/answer/semantic/answer.1.txt cases/result/semantic/result.1.txt
echo "[TEST 2]"
cases/diff_checker cases/answer/semantic/answer.2.txt cases/result/semantic/result.2.txt
echo "[TEST 3]"
cases/diff_checker cases/answer/semantic/answer.3.txt cases/result/semantic/result.3.txt
echo "[TEST 4]"
cases/diff_checker cases/answer/semantic/answer.4.txt cases/result/semantic/result.4.txt

echo ""
echo "[TEST END]"
make clean