#!/bin/bash
#g++ fib43.cpp -o fib43 -O2 && time ./fib43
g++ fib43.s -o /tmp/fib43.bin && time /tmp/fib43.bin
