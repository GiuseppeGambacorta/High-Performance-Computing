/****************************************************************************
 *
 * mpi-ring.c - Ring communication with MPI
 *
 * Copyright (C) 2017--2023 Moreno Marzolla <https://www.moreno.marzolla.name/>
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
% HPC - Ring communication with MPI
% [Moreno Marzolla](https://www.moreno.marzolla.name/)
% Last updated: 2023-11-06

Write a MPI program [mpi-ring.c](mpi-ring.c) that implements a message
exchange along a ring. Let $P$ be the number of MPI processes; the
program should behave according to the following specification:

- The program receives an integer $K \geq 1$ from the command
  line. $K$ is the number of "turns" of the ring. Since all MPI
  processes have access to the command line parameters, they know the
  value $K$ without the need to communicate.

- Process 0 (the master) sends process 1 an integer, whose
  initial value is 1.

- Every process $p$ (including the master) receives a value $v$ from
  the predecessor $p-1$, and sends $(v + 1)$ to the successor
  $p+1$. The predecessor of 0 is $(P - 1)$, and the successor of $(P -
  1)$ is 0.

- The master prints the value received after the $K$-th iteration and
  the program terminates. Given the number $P$ of processes and the
  value of $K$, what value should be printed by the master?

For example, if $K = 2$ and there are $P = 4$ processes, the
communication should be as shown in Figure 1 (arrows are messages
whose content is the number reported above them). There are $K = 2$
turns of the ring; at the end, process 0 receives and prints 8.

![Figure 1: Ring communication](mpi-ring.svg)

To compile:

        mpicc -std=c99 -Wall -Wpedantic mpi-ring.c -o mpi-ring

To execute:

        mpirun -n 4 ./mpi-ring

## Files

- [mpi-ring.c](mpi-ring.c)

***/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main( int argc, char *argv[] )
{
    int my_rank, comm_sz, K = 10;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );
    MPI_Comm_size( MPI_COMM_WORLD, &comm_sz );

    if ( argc > 1 ) {
        K = atoi(argv[1]);
    }

     MPI_Status status;

    if (my_rank==0){
      int round = 0;
      int number = 0;
      int toSend = (my_rank +1 < comm_sz? my_rank +1 : MPI_PROC_NULL);
      for (int i= 0; i <K ; i++){
        number++;
        
        printf("ingresso 0\n");
        MPI_Send(&number,1,MPI_INT,toSend,0,MPI_COMM_WORLD);
        printf("rank %d send %d at round %d\n", my_rank,number,round);
        MPI_Recv(&number,1,MPI_INT,comm_sz-1,0,MPI_COMM_WORLD,&status);
        printf("rank %d riceved %d at round %d\n", my_rank,number,round+1);
      }
      number = -1;
      MPI_Send(&number,1,MPI_INT,toSend,0,MPI_COMM_WORLD);
      printf("uscita finale 0\n");

    } else {
      int number = 0;
      int toSend = (my_rank +1 < comm_sz? my_rank +1 : 0);
      while (1){
        MPI_Recv(&number,1,MPI_INT,my_rank-1,0,MPI_COMM_WORLD,&status);
        if (number == -1){
          MPI_Send(&number,1,MPI_INT,toSend,0,MPI_COMM_WORLD);
          printf("uscita finale %d\n", my_rank);
          break;
        }
        printf("rank %d riceved %d\n", my_rank,number);
        number++;
        
        MPI_Send(&number,1,MPI_INT,toSend,0,MPI_COMM_WORLD);
      }
    }

    

    MPI_Finalize();
    return EXIT_SUCCESS;
}