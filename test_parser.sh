#!/bin/bash

make cminus_parser MAIN=test

# Make cimpl
echo "---MAKE PARSER---"
./cminus_parser cases/test/test.1.txt cases/result/parser/result.1.txt
echo "COMPILE 1 COMPLETE"
./cminus_parser cases/test/test.2.txt cases/result/parser/result.2.txt
echo "COMPILE 2 COMPLETE"
./cminus_parser cases/test/test.3.txt cases/result/parser/result.3.txt
echo "COMPILE 3 COMPLETE"
./cminus_parser cases/test/test.4.txt cases/result/parser/result.4.txt
echo "COMPILE 4 COMPLETE"
./cminus_parser cases/test/test.5.txt cases/result/parser/result.5.txt
echo "COMPILE 5 COMPLETE"
./cminus_parser cases/test/test.6.txt cases/result/parser/result.6.txt
echo "COMPILE 6 COMPLETE"
./cminus_parser cases/test/test.7.txt cases/result/parser/result.7.txt
echo "COMPILE 7 COMPLETE"
./cminus_parser cases/test/test.8.txt cases/result/parser/result.8.txt
echo "COMPILE 8 COMPLETE"
./cminus_parser cases/test/test.9.txt cases/result/parser/result.9.txt
echo "COMPILE 9 COMPLETE"

echo "[TEST 1]"
cases/diff_checker cases/answer/parser/answer.1.txt cases/result/parser/result.1.txt
echo "[TEST 2]"
cases/diff_checker cases/answer/parser/answer.2.txt cases/result/parser/result.2.txt

echo ""
echo "[TEST END]"
make clean