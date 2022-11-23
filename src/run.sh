#!/bin/bash -x
./predictor ../traces/int_1 "$@"
./predictor ../traces/int_2 "$@"
./predictor ../traces/fp_1 "$@"
./predictor ../traces/fp_2 "$@"
./predictor ../traces/mm_1 "$@"
./predictor ../traces/mm_2 "$@"
