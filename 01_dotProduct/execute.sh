#!/bin/bash


if [ $# -ne 1 ]; then
    OMP_PLACES="{0:1},{1:1},{2:1},{3:1},{4:1},{5:1},{6:1},{7:1},{8:1},{9:1},{10:1},{11:1}"
    OMP_PROC_BIND=true
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