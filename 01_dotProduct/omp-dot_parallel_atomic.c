#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>

void fill( int *v1, int *v2, size_t n )
{
    const int seq1[3] = { 3, 7, 18};
    const int seq2[3] = {12, 0, -2};
    for (size_t i=0; i<n; i++) {
        v1[i] = seq1[i%3];
        v2[i] = seq2[i%3];
    }
}

int dot(const int *v1, const int *v2, size_t n)
{
    int result = 0;

    #pragma omp parallel
    {
        int my_rank = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        size_t start = my_rank * n / num_threads;
        size_t end = (my_rank + 1) * n / num_threads;
        int local_result = 0;

        for (size_t i = start; i < end; i++) {
            local_result += v1[i] * v2[i];
        }

        #pragma omp atomic
            result += local_result;
    }

    return result;
}

int main( int argc, char *argv[] )
{
    size_t n = 512*1024*1024l; /* array length */
    const size_t n_max = 512*1024*1024l; /* max length */
    int *v1, *v2;

    if ( argc > 2 ) {
        fprintf(stderr, "Usage: %s [n]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if ( argc > 1 ) {
        n = atol(argv[1]);
    }

    if ( n > n_max ) {
        fprintf(stderr, "FATAL: Array too long (requested length=%lu, maximum length=%lu\n", (unsigned long)n, (unsigned long)n_max);
        return EXIT_FAILURE;
    }

    printf("Initializing array of length %lu\n", (unsigned long)n);
    printf("num of thread %d\n", omp_get_max_threads());
    v1 = (int*)malloc( n*sizeof(v1[0])); assert(v1 != NULL);
    v2 = (int*)malloc( n*sizeof(v2[0])); assert(v2 != NULL);
    fill(v1, v2, n);

    const int expect = (n % 3 == 0 ? 0 : 36);

    const double tstart = omp_get_wtime();
    const int result = dot(v1, v2, n);
    const double elapsed = omp_get_wtime() - tstart;

    if ( result == expect ) {
        printf("Test OK\n");
    } else {
        printf("Test FAILED: expected %d, got %d\n", expect, result);
    }
    printf("Elapsed time: %f\n", elapsed);
    free(v1);
    free(v2);

    return EXIT_SUCCESS;
}
