/****************************************************************************
 *
 * mpi-dot.c - Dot product
 *
 * Copyright (C) 2016--2023 Moreno Marzolla <https://www.moreno.marzolla.name/>
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
% HPC - Dot product
% [Moreno Marzolla](https://www.moreno.marzolla.name/)
% Last updated: 2023-11-15

The file [mpi-dot.c](mpi-dot.c) contains a MPI program that computes
the dot product between two arrays `x[]` and `y[]` of length $n$. The
dot product $s$ of two arrays `x[]` and `y[]` is defined as:

$$
s = \sum_{i = 0}^{n-1} x[i] \times y[i]
$$

In the provided program, the master performs the whole computation and
is therefore not parallel. The goal of this exercise is to write a
parallel version. Assume that, at the beginning of the program, `x[]`
and `y[]` are known only to the master. Therefore, they must be
distributed across the processes. Each process computes the scalar
product of the assigned portions of the arrays; the master then uses
`MPI_Reduce()` to sum the partial results and compute $s$.

You may initially assume that $n$ is an exact multiple of the number
of MPI processes $P$; then, relax this assumption and modify the
program so that it works with any array length $n$. The simpler
solution is to distribute the arrays using `MPI_Scatter()` and let the
master take care of any excess data. Another possibility is to use
`MPI_Scatterv()` to distribute the input unevenly across the
processes.

To compile:

        mpicc -std=c99 -Wall -Wpedantic mpi-dot.c -o mpi-dot -lm

To execute:

        mpirun -n P ./mpi-dot [n]

Example:

        mpirun -n 4 ./mpi-dot 1000

## Files

- [mpi-dot.c](mpi-dot.c)

***/
#include <stdio.h>
#include <stdlib.h>
#include <math.h> /* for fabs() */
#include <assert.h>
#include <mpi.h>

/*
 * Compute sum { x[i] * y[i] }, i=0, ... n-1
 */
double dot( const double* x, const double* y, int n )
{
    double s = 0.0;
    int i;
    for (i=0; i<n; i++) {
        s += x[i] * y[i];
    }
    return s;
}

int main( int argc, char* argv[] )
{
    const double TOL = 1e-5;
    double *x = NULL, *y = NULL, result = 0.0;
    int i, n = 1400;
    int my_rank, comm_sz;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if ( argc > 1 ) {
        n = atoi(argv[1]);
    }

    int local_n= n/comm_sz;
    int module = n % comm_sz;
    double* localx = (double*)malloc(local_n*sizeof(double));
    double* localy = (double*)malloc(local_n*sizeof(double));

    if ( 0 == my_rank ) {
        /* The master allocates the vectors */
        x = (double*)malloc( n * sizeof(*x) ); assert(x != NULL);
        y = (double*)malloc( n * sizeof(*y) ); assert(y != NULL);
        for ( i=0; i<n; i++ ) {
            x[i] = i + 1.0;
            y[i] = 1.0 / x[i];
        }
    }
    
    MPI_Scatter(x, local_n, MPI_DOUBLE, localx, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, local_n, MPI_DOUBLE, localy, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);


    double local_result = dot(localx,localy,local_n);
    printf("%d\n",local_result);


    MPI_Reduce(&local_result, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (0 == my_rank) {
        printf("Dot product: %f\n", result);
        if ( fabs(result - n) < TOL ) {
            printf("Check OK\n");
        } else {
            printf("Check failed: got %f, expected %f\n", result, (double)n);
        }
    }

    free(localx);
    free(localy);
    free(x); /* if x == NULL, does nothing */
    free(y);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
