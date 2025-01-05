/****************************************************************************
 *
 * mpi-my-bcast.c - Broadcast using point-to-point communications
 *
 * Copyright (C) 2017--2024 Moreno Marzolla <https://www.moreno.marzolla.name/>
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
% HPC - Broadcast using point-to-point communications
% [Moreno Marzolla](https://www.moreno.marzolla.name/)
% Last updated: 2024-10-31

The purpose of this exercise is to implement the function

        void my_Bcast(int *v)

using point-to-point communications. The function
sends the value `*v` that
resides on the local memory of process 0 to all other
processes. In practice, this function should be equivalent
to calling

```C
MPI_Bcast(v,             \/\* buffer   \*\/
          1,             \/\* count    \*\/
          MPI_INT,       \/\* datatype \*\/
          0,             \/\* root     \*\/
          MPI_COMM_WORLD \/\* comm     \*\/
          );
```

> **Note**. `MPI_Bcast()` must always be preferred to any home-made
> solution. The purpose of this exercise is to learn how `MPI_Bcast()`
> _might_ be implemented.

To implement `my_Bcast()`, each process determines its own rank $p$
and the number $P$ of MPI processes. Then, process 0:

- sends `*v` to processes $(2p + 1)$ and $(2p + 2)$, provided that
  they exist.

Any other process $p>0$:

- receives an integer from $(p - 1)/2$ and stores it in `*v`;

- sends `*v` to processes $(2p + 1)$ and $(2p + 2)$, provided that
  they exist.

For example, with $P = 15$ you get the communication pattern shown in
Figure 1; arrows indicate point-to-point communications, numbers
indicate the rank of processes. The procedure above should work
correctly for any $P$.

![Figure 1: Broadcast tree with $P = 15$ processes](mpi-my-bcast.svg)

The file [mpi-my-bcast.c](mpi-my-bcast.c) contains the skeleton of the
`my_Bcast()` function. Complete the implementation using
point-to-point send/receive operations.

To compile:

        mpicc -std=c99 -Wall -Wpedantic mpi-my-bcast.c -o mpi-my-bcast

To execute:

        mpirun -n 4 ./mpi-my-bcast

## Files

- [mpi-my-bcast.c](mpi-my-bcast.c)

***/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void my_Bcast(int *v)
{
    int my_rank, comm_sz;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int firstToSend = (my_rank*2 +1 < comm_sz? my_rank*2+1 : MPI_PROC_NULL);
    int secondToSend =(my_rank*2 +2 < comm_sz? my_rank*2+2 : MPI_PROC_NULL);
   

    if (my_rank == 0){
        MPI_Send(v,1,MPI_INT,firstToSend,0,MPI_COMM_WORLD);
        MPI_Send(v,1,MPI_INT,secondToSend,0,MPI_COMM_WORLD);
    } else {
        MPI_Status status;
        int reciveFrom = (my_rank-1)/2;
        MPI_Recv(v,1,MPI_INT,reciveFrom,0,MPI_COMM_WORLD, &status);

        MPI_Send(v,1,MPI_INT,firstToSend,0,MPI_COMM_WORLD);
        MPI_Send(v,1,MPI_INT,secondToSend,0,MPI_COMM_WORLD);
        

        
    }
}


int main( int argc, char *argv[] )
{
    const int SENDVAL = 123; /* value to be broadcasted */
    int my_rank;
    int v;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* only process 0 sets `v` to the value to be broadcasted. */
    if ( 0 == my_rank ) {
        v = SENDVAL;
    } else {
        v = -1;
    }

    printf("BEFORE: value of `v` at rank %d = %d\n", my_rank, v);

    my_Bcast(&v);

    if ( v == SENDVAL ) {
        printf("OK: ");
    } else {
        printf("ERROR: ");
    }
    printf("value of `v` at rank %d = %d\n", my_rank, v);

    MPI_Finalize();

    return EXIT_SUCCESS;
}