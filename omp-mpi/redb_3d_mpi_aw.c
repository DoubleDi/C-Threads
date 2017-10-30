#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define  Max(a,b) ((a)>(b)?(a):(b))
#define  Min(a,b) ((a)<(b)?(a):(b))
#define N (100)
double   maxeps = 0.1e-7;
int itmax = 100;
int i,j,k;
double w = 0.5;
double eps;

double (*A)[N][N];

void relax();
void init();
void verify();

int rank;
int numprocs;
int size;
int start;
int end;

double start_time;

MPI_Request req[4];
MPI_Status status[4];

int main(int an, char **as)
{
    MPI_Init(&an, &as);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    start = rank * N / numprocs;
    end = Min(N, (rank + 1) * N / numprocs + 2);
    size = end - start;
    printf("%d %d %d\n", start, end, size);

    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    A = malloc(size * N * N * sizeof(double));

	int it;

	init();

	for(it=1; it<=itmax; it++)
	{
		eps = 0.;
		relax();

        if (rank == 0) {
            printf( "it=%4i   eps=%f\n", it,eps);
        }
		if (eps < maxeps) break;
	}

	verify();

    if (rank == 0) {
        printf("Time elapsed: %lf\n", MPI_Wtime() - start_time);
    }

    MPI_Finalize();

	return 0;
}


void init()
{
	for(i=start; i < end; i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		if(i==0 || i==N-1 || j==0 || j==N-1 || k==0 || k==N-1)     A[i - start][j][k]= 0.;
		else A[i - start][j][k]= ( 4. + i + j + k) ;
	}
}


void relax()
{
    double eps_p = 0;

	for(i=1; i<=size - 2; i++)
	for(j=1; j<=N-2; j++)
	for(k=1+(i+start+j)%2; k<=N-2; k+=2)
	{
		double b;
		b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
		+A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
		eps_p =  Max(fabs(b),eps_p);
		A[i][j][k] = A[i][j][k] + b;
	}

    if (rank != 0) {
        MPI_Irecv(&A[0][0][0], N * N, MPI_DOUBLE, rank - 1, rank - 1, MPI_COMM_WORLD, &req[0]);
    }

    if (rank != numprocs - 1) {
        MPI_Isend(&A[size - 2][0][0], N * N, MPI_DOUBLE, rank + 1, rank, MPI_COMM_WORLD, &req[2]);
    }

    if (rank != numprocs - 1) {
        MPI_Irecv(&A[size - 1][0][0], N * N, MPI_DOUBLE, rank + 1, rank + 1, MPI_COMM_WORLD, &req[3]);
    }

    if (rank != 0) {
        MPI_Isend(&A[1][0][0], N * N, MPI_DOUBLE, rank - 1, rank, MPI_COMM_WORLD, &req[1]);
    }

    int num = 4, shift = 0;

    if (rank == 0) {
        num = 2;
        shift = 2;
    }

    if (rank == numprocs - 1) {
        num = 2;
    }

    if (numprocs > 1) {
        MPI_Waitall(num, &req[shift], &status[0]);
    }

    MPI_Allreduce(&eps_p, &eps, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

	for(i=1; i<= size - 2; i++)
	for(j=1; j<=N-2; j++)
	for(k=1+(i+j+1+start)%2; k<=N-2; k+=2)
	{
		double b;
		b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
		+A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
		A[i][j][k] = A[i][j][k] + b;
	}

    if (rank != 0) {
        MPI_Irecv(&A[0][0][0], N * N, MPI_DOUBLE, rank - 1, rank - 1, MPI_COMM_WORLD, &req[0]);
    }

    if (rank != numprocs - 1) {
        MPI_Isend(&A[size - 2][0][0], N * N, MPI_DOUBLE, rank + 1, rank, MPI_COMM_WORLD, &req[2]);
    }

    if (rank != numprocs - 1) {
        MPI_Irecv(&A[size - 1][0][0], N * N, MPI_DOUBLE, rank + 1, rank + 1, MPI_COMM_WORLD, &req[3]);
    }

    if (rank != 0) {
        MPI_Isend(&A[1][0][0], N * N, MPI_DOUBLE, rank - 1, rank, MPI_COMM_WORLD, &req[1]);
    }

    num = 4, shift = 0;

    if (rank == 0) {
        num = 2;
        shift = 2;
    }

    if (rank == numprocs - 1) {
        num = 2;
    }

    if (numprocs > 1) {
        MPI_Waitall(num, &req[shift], &status[0]);
    }

}


void verify()
{
	double s;

	s=0.;
	for(i=0; i< (rank == numprocs - 1 ? size : size - 2); i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		s=s+A[i][j][k]*(i + start +1)*(j+1)*(k+1)/(N*N*N);
	}

    double sum = 0;

    MPI_Reduce(&s, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
	    printf("  S = %f\n",sum);
    }

}
