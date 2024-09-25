#!/bin/bash

# cimpl 테스트
echo "---CIMPL TEST---"
./cminus_cimpl tests/test.1.txt tests/result.1.cimpl.txt
./cminus_cimpl tests/test.2.txt tests/result.2.cimpl.txt

echo "[TEST 1]"
tests/diff_checker tests/answer.1.txt tests/result.1.cimpl.txt
echo "[TEST 2]"
tests/diff_checker tests/answer.2.txt tests/result.2.cimpl.txt

# lex 테스트
echo ""
echo "---LEX TEST---"
./cminus_lex tests/test.1.txt tests/result.1.lex.txt
./cminus_lex tests/test.2.txt tests/result.2.lex.txt

echo "[TEST 1]"
tests/diff_checker tests/answer.1.txt tests/result.1.cimpl.txt
echo "[TEST 2]"
tests/diff_checker tests/answer.2.txt tests/result.2.cimpl.txt