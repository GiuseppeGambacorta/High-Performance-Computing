#!/bin/bash

if [ $# -lt 3 ]; then
    echo "Usage: $0 <folder> <core_type> Optional: <num_cores>"
    exit 1
fi

FOLDER=$1

if [ ! -d "$FOLDER" ]; then
    echo "The specified folder does not exist."
    exit 1
fi


if [ "$2" == "P" ]; then
    PLACES="{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11}"
    CORENUM=12
elif [ "$2" == "E" ]; then
    PLACES="{12},{13},{14},{15},{16},{17},{18},{19}"
    CORENUM=8
else
     PLACES="{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17},{18},{19}"
fi

if [ $# -ge 3 ]; then
    CORENUM=$3
fi

OMP_PROC=true


files=$(find "$FOLDER" -maxdepth 1 -name "*.c")


for file in $files; do
    gcc -std=c99 -Wall -Wextra -pedantic -fopenmp "$file" -o "${file%.c}"
done

for file in $files; do
    echo "Running ${file%.c} with ${CORENUM}  cores"
    OMP_NUM_THREADS=$CORENUM OMP_PROC_BIND=$OMP_PROC OMP_PLACES=$PLACES ./"${file%.c}"
done

for file in $files; do
    rm ./"${file%.c}"
done