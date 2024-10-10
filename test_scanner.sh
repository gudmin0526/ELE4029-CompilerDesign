#!/bin/bash

make all

# Make cimpl
echo "---MAKE CIMPL---"
./cminus_cimpl cases/test/test.1.txt cases/result/scanner/result.1.cimpl.txt
echo "COMPILE 1 COMPLETE"
./cminus_cimpl cases/test/test.2.txt cases/result/scanner/result.2.cimpl.txt
echo "COMPILE 2 COMPLETE"
./cminus_cimpl cases/test/test.3.txt cases/result/scanner/result.3.cimpl.txt
echo "COMPILE 3 COMPLETE"
./cminus_cimpl cases/test/test.4.txt cases/result/scanner/result.4.cimpl.txt
echo "COMPILE 4 COMPLETE"
./cminus_cimpl cases/test/test.5.txt cases/result/scanner/result.5.cimpl.txt
echo "COMPILE 5 COMPLETE"
./cminus_cimpl cases/test/test.6.txt cases/result/scanner/result.6.cimpl.txt
echo "COMPILE 6 COMPLETE"
./cminus_cimpl cases/test/test.7.txt cases/result/scanner/result.7.cimpl.txt
echo "COMPILE 7 COMPLETE"
./cminus_cimpl cases/test/test.8.txt cases/result/scanner/result.8.cimpl.txt
echo "COMPILE 8 COMPLETE"
./cminus_cimpl cases/test/test.9.txt cases/result/scanner/result.9.cimpl.txt
echo "COMPILE 9 COMPLETE"

# Make lex
echo ""
echo "---MAKE LEX---"
./cminus_lex cases/test/test.1.txt cases/result/scanner/result.1.lex.txt
echo "COMPILE 1 COMPLETE"
./cminus_lex cases/test/test.2.txt cases/result/scanner/result.2.lex.txt
echo "COMPILE 2 COMPLETE"
./cminus_lex cases/test/test.3.txt cases/result/scanner/result.3.lex.txt
echo "COMPILE 3 COMPLETE"
./cminus_lex cases/test/test.4.txt cases/result/scanner/result.4.lex.txt
echo "COMPILE 4 COMPLETE"
./cminus_lex cases/test/test.5.txt cases/result/scanner/result.5.lex.txt
echo "COMPILE 5 COMPLETE"
./cminus_lex cases/test/test.6.txt cases/result/scanner/result.6.lex.txt
echo "COMPILE 6 COMPLETE"
./cminus_lex cases/test/test.7.txt cases/result/scanner/result.7.lex.txt
echo "COMPILE 7 COMPLETE"
./cminus_lex cases/test/test.8.txt cases/result/scanner/result.8.lex.txt
echo "COMPILE 8 COMPLETE"
./cminus_lex cases/test/test.9.txt cases/result/scanner/result.9.lex.txt
echo "COMPILE 9 COMPLETE"

echo ""
echo "[TEST START]"
echo "[TEST 1]"
cases/diff_checker cases/result/scanner/result.1.lex.txt cases/result/scanner/result.1.cimpl.txt
echo "[TEST 2]"
cases/diff_checker cases/result/scanner/result.2.lex.txt cases/result/scanner/result.2.cimpl.txt
echo "[TEST 3]"
cases/diff_checker cases/result/scanner/result.3.lex.txt cases/result/scanner/result.3.cimpl.txt
echo "[TEST 4]"
cases/diff_checker cases/result/scanner/result.4.lex.txt cases/result/scanner/result.4.cimpl.txt
echo "[TEST 5]"
cases/diff_checker cases/result/scanner/result.5.lex.txt cases/result/scanner/result.5.cimpl.txt
echo "[TEST 6]"
cases/diff_checker cases/result/scanner/result.6.lex.txt cases/result/scanner/result.6.cimpl.txt
echo "[TEST 7]"
cases/diff_checker cases/result/scanner/result.7.lex.txt cases/result/scanner/result.7.cimpl.txt
echo "[TEST 8]"
cases/diff_checker cases/result/scanner/result.8.lex.txt cases/result/scanner/result.8.cimpl.txt
echo "[TEST 9]"
cases/diff_checker cases/result/scanner/result.9.lex.txt cases/result/scanner/result.9.cimpl.txt

echo "[TEST A]"
cases/diff_checker cases/answer/scanner/answer.1.txt cases/result/scanner/result.1.cimpl.txt
echo "[TEST B]"
cases/diff_checker cases/answer/scanner/answer.2.txt cases/result/scanner/result.2.cimpl.txt

echo ""
echo "[TEST END]"
make clean