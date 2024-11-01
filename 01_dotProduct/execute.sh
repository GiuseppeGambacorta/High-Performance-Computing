#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <number_of_cores>"
    exit 1
fi

NUMBER_OF_CORE=$1
files=("omp-dot_parallelForReduce.c" "omp-dot_parallel_reduce.c" "omp-dot_parallel_atomic.c" "omp-dot_parallel_critical.c")

for file in "${files[@]}"; do
    gcc -std=c99 -Wall -Wextra -pedantic -fopenmp "$file" -o "${file%.c}"
done

for file in "${files[@]}"; do
    echo "Running ${file%.c} with $NUMBER_OF_CORE cores"
    OMP_NUM_THREADS=$NUMBER_OF_CORE ./"${file%.c}"
done


for file in "${files[@]}"; do
    rm ./"${file%.c}"
done