#!/bin/bash

make clean
make all

# cimpl 테스트
echo "---CIMPL TEST---"
./cminus_cimpl tests/test.1.txt tests/result.1.cimpl.txt
echo "COMPILE 1 COMPLETE"
./cminus_cimpl tests/test.2.txt tests/result.2.cimpl.txt
echo "COMPILE 2 COMPLETE"
./cminus_cimpl tests/test.3.txt tests/result.3.cimpl.txt
echo "COMPILE 3 COMPLETE"
# ./cminus_cimpl tests/test.4.txt tests/result.4.cimpl.txt
# echo "COMPILE 4 COMPLETE"
# ./cminus_cimpl tests/test.5.txt tests/result.5.cimpl.txt
# echo "COMPILE 5 COMPLETE"
# ./cminus_cimpl tests/test.6.txt tests/result.6.cimpl.txt
# echo "COMPILE 6 COMPLETE"
# ./cminus_cimpl tests/test.7.txt tests/result.7.cimpl.txt
# echo "COMPILE 7 COMPLETE"
# ./cminus_cimpl tests/test.8.txt tests/result.8.cimpl.txt
# echo "COMPILE 8 COMPLETE"
# ./cminus_cimpl tests/test.9.txt tests/result.9.cimpl.txt
# echo "COMPILE 9 COMPLETE"
# ./cminus_cimpl tests/test.10.txt tests/result.10.cimpl.txt
# echo "COMPILE 10 COMPLETE"

echo "[TEST 1]"
tests/diff_checker tests/answer.1.txt tests/result.1.cimpl.txt
echo "[TEST 2]"
tests/diff_checker tests/answer.2.txt tests/result.2.cimpl.txt
echo "[TEST 3]"
tests/diff_checker tests/answer.3.txt tests/result.3.cimpl.txt
# echo "[TEST 4]"
# tests/diff_checker tests/answer.4.txt tests/result.4.cimpl.txt
# echo "[TEST 5]"
# tests/diff_checker tests/answer.5.txt tests/result.5.cimpl.txt
# echo "[TEST 6]"
# tests/diff_checker tests/answer.6.txt tests/result.6.cimpl.txt
# echo "[TEST 7]"
# tests/diff_checker tests/answer.7.txt tests/result.7.cimpl.txt
# echo "[TEST 8]"
# tests/diff_checker tests/answer.8.txt tests/result.8.cimpl.txt
# echo "[TEST 9]"
# tests/diff_checker tests/answer.9.txt tests/result.9.cimpl.txt
# echo "[TEST 10]"
# tests/diff_checker tests/answer.10.txt tests/result.10.cimpl.txt

# lex 테스트
echo ""
echo "---LEX TEST---"
./cminus_lex tests/test.1.txt tests/result.1.lex.txt
echo "COMPILE 1 COMPLETE"
./cminus_lex tests/test.2.txt tests/result.2.lex.txt
echo "COMPILE 2 COMPLETE"
./cminus_lex tests/test.3.txt tests/result.3.lex.txt
echo "COMPILE 3 COMPLETE"
# ./cminus_lex tests/test.4.txt tests/result.4.lex.txt
# echo "COMPILE 4 COMPLETE"
# ./cminus_lex tests/test.5.txt tests/result.5.lex.txt
# echo "COMPILE 5 COMPLETE"
# ./cminus_lex tests/test.6.txt tests/result.6.lex.txt
# echo "COMPILE 6 COMPLETE"
# ./cminus_lex tests/test.7.txt tests/result.7.lex.txt
# echo "COMPILE 7 COMPLETE"
# ./cminus_lex tests/test.8.txt tests/result.8.lex.txt
# echo "COMPILE 8 COMPLETE"
# ./cminus_lex tests/test.9.txt tests/result.9.lex.txt
# echo "COMPILE 9 COMPLETE"
# ./cminus_lex tests/test.10.txt tests/result.10.lex.txt
# echo "COMPILE 10 COMPLETE"

echo "[TEST 1]"
tests/diff_checker tests/answer.1.txt tests/result.1.cimpl.txt
echo "[TEST 2]"
tests/diff_checker tests/answer.2.txt tests/result.2.cimpl.txt
echo "[TEST 3]"
tests/diff_checker tests/answer.3.txt tests/result.3.cimpl.txt
# echo "[TEST 4]"
# tests/diff_checker tests/answer.4.txt tests/result.4.cimpl.txt
# echo "[TEST 5]"
# tests/diff_checker tests/answer.5.txt tests/result.5.cimpl.txt
# echo "[TEST 6]"
# tests/diff_checker tests/answer.6.txt tests/result.6.cimpl.txt
# echo "[TEST 7]"
# tests/diff_checker tests/answer.7.txt tests/result.7.cimpl.txt
# echo "[TEST 8]"
# tests/diff_checker tests/answer.8.txt tests/result.8.cimpl.txt
# echo "[TEST 9]"
# tests/diff_checker tests/answer.9.txt tests/result.9.cimpl.txt
# echo "[TEST 10]"
# tests/diff_checker tests/answer.10.txt tests/result.10.cimpl.txt