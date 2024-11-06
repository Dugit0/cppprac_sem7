#!/usr/bin/env bash

function cmcgpp() {
    outfile=${1%%.*}
    echo "${outfile}.out"
    g++ -fsanitize=address,undefined,signed-integer-overflow,pointer-compare,pointer-subtract,leak,bounds,pointer-overflow -O2 -Wall -Wextra -std=c++23 -lm -o "${outfile}.out" ${1}
}

echo "======== COMPILE ========" &&
cmcgpp parallel_main.cpp &&
echo "========== RUN ==========" &&
./parallel_main.out 'inputs/input.csv' 4

# echo "============ ============"
