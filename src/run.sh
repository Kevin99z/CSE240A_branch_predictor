#!/bin/bash -x


# ./predictor ../traces/fp_1 --gshare:13
./predictor ../traces/fp_1 --custom:14:13:10
# ./predictor ../traces/fp_1 --tournament:9:10:10
# ./predictor ../traces/fp_2 --gshare:13
./predictor ../traces/fp_2 --custom:14:13:10
# ./predictor ../traces/fp_2 --tournament:9:10:10
# ./predictor ../traces/int_1 --gshare:13
./predictor ../traces/int_1 --custom:14:13:10
# ./predictor ../traces/int_1 --tournament:9:10:10
# ./predictor ../traces/int_2 --gshare:13
./predictor ../traces/int_2 --custom:14:13:10
# ./predictor ../traces/int_2 --tournament:9:10:10
# ./predictor ../traces/mm_1 --gshare:13
./predictor ../traces/mm_1 --custom:14:13:10
# ./predictor ../traces/mm_1 --tournament:9:10:10
# ./predictor ../traces/mm_2 --gshare:13
./predictor ../traces/mm_2 --custom:14:13:10
# ./predictor ../traces/mm_2 --tournament:9:10:10
./predictor ../traces/mm_2 --static

# ./predictor ../traces/fp_1 "$@"
# ./predictor ../traces/fp_2 "$@"
# ./predictor ../traces/int_1 "$@"
# ./predictor ../traces/int_2 "$@"
# ./predictor ../traces/mm_1 "$@"
# ./predictor ../traces/mm_2 "$@"
