#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: $0 <folder> [core_type] [num_cores] [num]"
    exit 1
fi

FOLDER=$1

if [ ! -d "$FOLDER" ]; then
    echo "The specified folder does not exist."
    exit 1
fi

if [ $# -ge 2 ]; then
    if [ "$2" == "P" ]; then
        export OMP_PLACES="{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11}"
    elif [ "$2" == "E" ]; then
        export OMP_PLACES="{12},{13},{14},{15},{16},{17},{18},{19}"
    else
        echo "Invalid core type. Use 'P' for P-cores or 'E' for E-cores."
        exit 1
    fi
else
    export OMP_PLACES="{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11}"
fi

export OMP_PROC_BIND=true

if [ $# -ge 3 ]; then
    export OMP_NUM_THREADS=$3
else
    export OMP_NUM_THREADS=12
fi

if [ $# -ge 4 ]; then
    NUM=$4
else
    echo "Write the number for Fibonacci"
    exit 1
fi

files=$(find "$FOLDER" -maxdepth 1 -name "*.c")

for file in $files; do
    gcc -std=c99 -Wall -Wextra -pedantic -fopenmp "$file" -o "${file%.c}"
done

for file in $files; do
    echo "Running ${file%.c} with ${OMP_NUM_THREADS} cores and NUM=$NUM"
    ./"${file%.c}" $NUM
done

for file in $files; do
    rm ./"${file%.c}"
done