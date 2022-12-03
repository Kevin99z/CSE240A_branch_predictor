#!/bin/bash -x

./predictor ../traces/int_1 --tournament:13:13:11
./predictor ../traces/int_1 --custom
./predictor ../traces/int_2 --tournament:13:13:11
./predictor ../traces/int_2 --custom
./predictor ../traces/fp_1 --tournament:13:13:11
./predictor ../traces/fp_1 --custom
./predictor ../traces/fp_2 --tournament:13:13:11
./predictor ../traces/fp_2 --custom
./predictor ../traces/mm_1 --tournament:13:13:11
./predictor ../traces/mm_1 --custom
./predictor ../traces/mm_2 --tournament:13:13:11
./predictor ../traces/mm_2 --custom

# ./predictor ../traces/int_1 "$@"
# ./predictor ../traces/int_2 "$@"
# ./predictor ../traces/fp_1 "$@"
# ./predictor ../traces/fp_2 "$@"
# ./predictor ../traces/mm_1 "$@"
# ./predictor ../traces/mm_2 "$@"
