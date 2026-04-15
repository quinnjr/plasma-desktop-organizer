#!/bin/sh
# Wrapper so lcov can use llvm-cov as a gcov-compatible tool.
# lcov passes all gcov arguments directly; llvm-cov gcov accepts them.
exec llvm-cov gcov "$@"
