#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> /* for rand() */
#include <time.h>   /* for time() */
#include <math.h>   /* for fabs() */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Generate `n` random points within the square with corners (-1, -1),
   (1, 1); return the number of points that fall inside the circle
   centered ad the origin with radius 1 */
int generate_points( int n )
{
    int n_inside = 0;
    for (int i = 0; i < n; i++) {
        const double x = (rand()/(double)RAND_MAX * 2.0) - 1.0;
        const double y = (rand()/(double)RAND_MAX * 2.0) - 1.0;
        if ( x*x + y*y < 1.0 ) {
            n_inside++;
        }
    }
    return n_inside;
}

int main( int argc, char *argv[] )
{
    int my_rank, comm_sz;
    int inside = 0, total_inside = 0, npoints = 1400000;
    double pi_approx;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (argc > 1) {
        npoints = atoi(argv[1]);
    }
    int points_per_process = npoints / comm_sz;

    // Seed the random number generator
    srand(my_rank * 11 + 7);

    if (my_rank ==0){
        int module = npoints % comm_sz;
        inside = generate_points(points_per_process+module);
    } else{
        inside = generate_points(points_per_process);
    }
    
                                                            //master
    MPI_Reduce(&inside, &total_inside, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        pi_approx = 4.0 * total_inside / (double)npoints;
        printf("PI approximation is %f (true value=%f, rel error=%.3f%%)\n", pi_approx, M_PI, 100.0 * fabs(pi_approx - M_PI) / M_PI);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
