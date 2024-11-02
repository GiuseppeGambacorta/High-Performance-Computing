/****************************************************************************
 *
 * omp-schedule.c - simulate the "schedule()" clause
 *
 * Copyright (C) 2017--2024 by Moreno Marzolla <https://www.moreno.marzolla.name/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

/***
% HPC - Simulate the "schedule()" clause
% [Moreno Marzolla](https://www.moreno.marzolla.name/)
% Last updated: 2024-10-28

OpenMP allows the use of the `schedule(static)` and
`schedule(dynamic)` clauses to choose how to assign loop iterations to
OpenMP threads. The purpose of this exercise is to simulate these
clauses using the `omp parallel` construct only.

The file [omp-schedule.c](omp-schedule.c) contains a serial program
that creates two arrays `vin[]` and `vout[]` of length $n$ such that
`vout[i] = Fib(vin[i])` for each $i$, where `Fib(k)` the _k_-th number
of the Fibonacci sequence. `Fib(k)` is intentionally computed using
the inefficient recursive algorithm, so that the computation time
varies widely depending on $k$.

There are two functions, `do_static()` and `do_dynamic()` that perform
the computation above.

1. Modify `do_static()` to distribute the loop iterations as would be
   done by the `schedule(static, chunk_size)` clause using the `omp
   parallel` directive (not `omp for`).

2. Modify `do_dynamic()` to distribute the loop iterations according
   to the _master-worker_ paradigm, as would be done by the
   `schedule(dynamic, chunk_size)` clause. Again, use the `omp
   parallel` directive (not `omp for`).

See the source code for suggestions.

To compile:

        gcc -std=c99 -Wall -Wpedantic -fopenmp omp-schedule.c -o omp-schedule

To execute:

        ./omp-schedule [n]

Example:

        OMP_NUM_THREADS=2 ./omp-schedule

## Files

- [omp-schedule.c](omp-schedule.c)

***/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>

/* Recursive computation of the n-th Fibonacci number, for n=0, 1, 2, ...
   Do not parallelize this function. */
int fib_rec(int n)
{
    if (n<2) {
        return 1;
    } else {
        return fib_rec(n-1) + fib_rec(n-2);
    }
}

/* Iterative computation of the n-th Fibonacci number. This function
   must be used for checking the result only. */
int fib_iter(int n)
{
    if (n<2) {
        return 1;
    } else {
        int fibnm1 = 1;
        int fibnm2 = 1;
        int fibn;
        n = n-1;
        do {
            fibn = fibnm1 + fibnm2;
            fibnm2 = fibnm1;
            fibnm1 = fibn;
            n--;
        } while (n>0);
        return fibn;
    }
}

/* Fill vectors `vin` and `vout` of length `n`; `vin` will contain
   input values; `vout` is initialized with -1 */
void fill(int *vin, int *vout, int n)
{
    for (int i=0; i<n; i++) {
        vin[i] = 25 + (i%10);
        vout[i] = -1;
    }
}

/* Check correctness of `vout[]`. Return 1 if correct, 0 if not */
int is_correct(const int *vin, const int *vout, int n)
{
    for (int i=0; i<n; i++) {
        if ( vout[i] != fib_iter(vin[i]) ) {
            fprintf(stderr,
                    "Test FAILED: vin[%d]=%d, vout[%d]=%d (expected %d)\n",
                    i, vin[i], i, vout[i], fib_iter(vin[i]));
            return 0;
        }
    }
    fprintf(stderr, "Test OK\n");
    return 1;
}


void do_static(const int *vin, int *vout, int n)
{
    /* [TODO] parallelize the following loop, simulating a
       "schedule(static,chunk_size)" clause. You should do so using
       "omp parallel" without "omp for".

       Hint: the iteration space i=0..n-1 should be partitioned into
       blocks of length `chunk_size`.  the blocks are assigned to
       OpenMP threads using a cyclic assignment, e.g.:

        <---------- STRIDE --------->
        +----------+----------+-----+----------+----------+-----
       0|    P0    |    P1    | ... |    P0    |    P1    | ...
        +----------+----------+-----+----------+----------+-----
         chunk_size chunk_size       chunk_size chunk_size

       Let `STRIDE` be the number of iterations between the beginning
       of a chunk assigned to thread `p` and the next chunk assigned
       to the same thread. Therefore:

       STRIDE = num_threads * chunk_size;

       The first chunk assigned to `p` starts at `(p * chunk_size)`.
       Therefore, each thread should execute the following nested
       loops:

       START = my_id * chunk_size;
       STRIDE = num_threads * chunk_size;
       for (int i=START; i<n; i+=STRIDE) {
         for (int j=i; j<i+chunk_size && j<n; j++) {
           loop body
         }
       }

       Note that `n` is not necessarily an integer multiple of the
       number of threads, therefore addtional checks are necessary to
       ensure that we never exceed the upper bound `n-1`.
    */
    const int chunk_size = 10; /* can be set to any value >= 1 */
    int number_of_chunks=0;
    int hopSize = 0;
    int resto = 0;
    if (chunk_size){
        number_of_chunks = n/ chunk_size;
        hopSize = n / number_of_chunks;
        resto = n%chunk_size;
    }
    int arraycheck[n];
    for (int i = 0; i < n; i++){
        arraycheck[i] = 0;
    }

    printf("number of chunks %d",number_of_chunks);
    printf("hopSize %d",hopSize);
    printf("resto %d\n",resto);
    
    #pragma omp parallel default(none) shared(n, vout, vin, chunk_size, number_of_chunks, resto, hopSize, arraycheck)
    {
        int my_id = omp_get_thread_num();
        int number_of_threads = omp_get_num_threads();
        int start;
        int end;

        if (chunk_size == 0) {
            start = (n * my_id) / number_of_threads;
            end = (n * (my_id + 1)) / number_of_threads;
        } else {
            start = (my_id * hopSize);
            end = ((my_id + 1) * hopSize) - 1;

            #pragma omp critical
            {
                printf("Thread %d: start %d, end %d\n", my_id, start, end);
            }
            #pragma omp barrier
        }

        // Ciclo while per iterare sui blocchi successivi
        while (start < n) {
            for (int i = start; i <= end && i < n; i++) {
                    if (i >= n) break;
                if (arraycheck[i]) {
                   
                        printf("Thread %d is processing the same element %d\n", my_id, i);
                
                }
                vout[i] = fib_rec(vin[i]);
                arraycheck[i] = 1;

                if (i>=n){
                    printf("superato n\n");
                }
             
               if (my_id == 3) {
                printf("Thread %d sta iniziando e finendo a %d %d\n", my_id, start, end);
            }
            }

            // Aggiorna start ed end per il prossimo blocco
            start += hopSize * number_of_threads;
            end += hopSize * number_of_threads;

          

            // Interrompe il ciclo se superiamo n
            if (start >= n) break;
            if (end >= n) break;
        }

        #pragma omp barrier
        if (my_id == 0) {
             for (int i = 0; i < n; i++) {
            if (arraycheck[i] == 0) {
                printf("Thread %d non ha processato l'elemento %d\n", my_id, i);
            }
        }
        }
       
    }

}

void do_dynamic(const int *vin, int *vout, int n)
{
    /* [TODO] parallelize the following loop, simulating a
       "schedule(dynamic,chunk_size)" clause. You should do so using
       "omp parallel" only, without "omp for".

       Hint: keep a shared variable `idx` representing the index of
       the beginning of the first unprocessed chunk, i.e., the first
       chunk that will be assigned to a thread.

       Each OpenMP thread _atomically_ fetches the current value of
       `idx` into a local (private) variable `my_idx`, and then
       increments `idx` by `chunk_size`.

       Therefore, each thread executes the following pseudocode:

       do {
         atomically copy idx into my_idx and increment idx by chunk_size
         for (i=my_idx; i<my_idx + chunk_size && i<n; i++) {
           loop body
         }
       } while (my_idx < n);
    */
    const int chunk_size = 1; /* can be set to any value >= 1 */
#pragma omp parallel for schedule(dynamic, chunk_size)
    for (int i=0; i<n; i++) {
        vout[i] = fib_rec(vin[i]);
        /* printf("vin[%d]=%d vout[%d]=%d\n", i, vin[i], i, vout[i]); */
    }
}

int main( int argc, char* argv[] )
{
    int n = 1024;
    const int max_n = 512*1024*1024;
    int *vin, *vout;
    double tstart, elapsed;

    if ( argc > 2 ) {
        fprintf(stderr, "Usage: %s [n]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if ( argc > 1 ) {
        n = atoi(argv[1]);
    }

    if ( n > max_n ) {
        fprintf(stderr, "FATAL: n too large (max value is %d)\n", max_n);
        return EXIT_FAILURE;
    }

    /* initialize the input and output arrays */
    vin = (int*)malloc(n * sizeof(vin[0])); assert(vin != NULL);
    vout = (int*)malloc(n * sizeof(vout[0])); assert(vout != NULL);

    /**
     ** Test static schedule implementation
     **/
    fill(vin, vout, n);
    tstart = omp_get_wtime();
    do_static(vin, vout, n);
    elapsed = omp_get_wtime() - tstart;
    is_correct(vin, vout, n);

    printf("Elapsed time (static schedule): %f\n", elapsed);

    /**
     ** Test dynamic schedule implementation
     **/
    fill(vin, vout, n);
    tstart = omp_get_wtime();
    do_dynamic(vin, vout, n);
    elapsed = omp_get_wtime() - tstart;
    is_correct(vin, vout, n);

    printf("Elapsed time (dynamic schedule): %f\n", elapsed);

    free(vin);
    free(vout);
    return EXIT_SUCCESS;
}
