#!/bin/bash



export OMP_PLACES="{0:1},{1:1},{2:1},{3:1},{4:1},{5:1},{6:1},{7:1},{8:1},{9:1},{10:1},{11:1}"
export OMP_PROC_BIND=true
export OMP_NUM_THREADS=12



if [ $# -ne 1 ]; then
    NUMBER_OF_CORE=$OMP_NUM_THREADS
else
    NUMBER_OF_CORE=$1
fi

files=$(find . -maxdepth 1 -name "*.c")

for file in $files; do
    gcc -std=c99 -Wall -Wextra -pedantic -fopenmp "$file" -o "${file%.c}"
done

for file in $files; do
    echo "Running ${file%.c} with $NUMBER_OF_CORE cores"
    OMP_NUM_THREADS=$NUMBER_OF_CORE ./"${file%.c}"
done


for file in $files; do
    rm ./"${file%.c}"
done