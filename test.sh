#!/bin/sh

BIN_NAME="woosh-d"
make debug

./$BIN_NAME test-script-exit0
if [ $? -ne 0 ]; then echo "test-script-exit0 failed $?" && exit 1; fi

./$BIN_NAME test-script-exit1
if [ $? -eq 0 ]; then echo "test-script-exit1 failed $?" && exit 1; fi

valgrind --leak-check=full --tool=memcheck -s ./$BIN_NAME test-script-exit0
if [ $? -ne 0 ]; then echo "test-script-exit0 failed valgrind $?" && exit 1; fi

valgrind --leak-check=full --tool=memcheck -s ./$BIN_NAME test-script-exit1
if [ $? -eq 0 ]; then echo "test-script-exit1 failed valgrind $?" && exit 1; fi

echo "all tests passed"
