gcc -std=c99 -Wall -Wpedantic -fopenmp omp-c-ray.c -o omp-c-ray -lm

./omp-c-ray -s 800x600 < sphfract.small.in > img.ppm


convert img.ppm img.jpeg
