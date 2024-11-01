#!/bin/bash


if [ $# -ne 1 ]; then
    OMP_PLACES="{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11}"
    OMP_PROC_BIND=master #false, spread
    OMP_NUM_THREADS=12
else    
    OMP_NUM_THREADS=$1
fi

files=$(find . -maxdepth 1 -name "*.c")

for file in $files; do
    gcc -std=c99 -Wall -Wextra -pedantic -fopenmp "$file" -o "${file%.c}"
done

for file in $files; do
    echo "Running ${file%.c} with ${OMP_NUM_THREADS} cores"
    ./"${file%.c}"
done


for file in $files; do
    rm ./"${file%.c}"
done