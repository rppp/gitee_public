#!/bin/bash
#g++ fib43.cpp -o fib43 -O2 && time ./fib43
g++ fib43.s -o fib43.bin && time ./fib43.bin
