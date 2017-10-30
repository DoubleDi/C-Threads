#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>


#define  Max(a,b) ((a)>(b)?(a):(b))
#define  Min(a,b) ((a)>(b)?(b):(a))

#define  N 100
double   maxeps = 0.1e-7;
int itmax = 100;
int i,j,k,size,rank;
int first,last,nrow,f,l;
double w = 0.5;
double eps;
double A[N][N][N];
MPI_Request req[4];
MPI_Status status[4];

void relax();
void init();
void verify();
void updatematrix();
void waitproccount();

int main(int an, char **as)
{
    int it, t1, t2;
    MPI_Init(&an, &as);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    first = (rank * N) / size;
    last = Min(N,(((rank + 1) * N) / size)+2);
    nrow = last - first;

    init();

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        t1 = MPI_Wtime();
    }

    //основная вычислительная секция
    for(it=1; it<=itmax; it++)
    {
        eps = 0.;
        relax();
        if (rank == 0) {
            printf( "it=%4i   eps=%f\n", it,eps);
            if (eps < maxeps) break;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        t2 = MPI_Wtime();
    }

    verify();


    if (rank == 0) {
        printf("PROCS: %d TIME: %d\n", size, t2-t1);
    }

    MPI_Finalize();
    return 0;
}


void init()
{
	for(i=first; i< last; i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		if(i==0 || i==N-1 || j==0 || j==N-1 || k==0 || k==N-1)     A[i-first][j][k]= 0.;
		else A[i-first][j][k]= ( 4. + i + j + k) ;
	}
}

void updatematrix() {
    if (rank != 0) {
        MPI_Irecv(&A[0][0][0], N * N, MPI_DOUBLE, rank - 1, rank - 1, MPI_COMM_WORLD, &req[0]);
    }
    if (rank != size - 1) {
        MPI_Isend(&A[nrow - 2][0][0], N * N, MPI_DOUBLE, rank + 1, rank, MPI_COMM_WORLD, &req[2]);
    }
    if (rank != size - 1) {
        MPI_Irecv(&A[nrow - 1][0][0], N * N, MPI_DOUBLE, rank + 1, rank + 1, MPI_COMM_WORLD, &req[3]);
    }
    if (rank != 0) {
        MPI_Isend(&A[1][0][0], N * N, MPI_DOUBLE, rank - 1, rank, MPI_COMM_WORLD, &req[1]);
    }
}

void waitproccount() {
    if (rank == 0) {
        f = 2;
        l = 2;
    } else if (rank == size - 1) {
        f = 2;
        l = 0;
    } else {
        f = 4;
        l = 0;
    }
}

void relax()
{
    double eps_i = eps;

	for(i=1; i<=nrow-2; i++)
	for(j=1; j<=N-2; j++)
	for(k=1+(i+j+first)%2; k<=N-2; k+=2)
	{
		double b;
		b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
		+A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
        eps_i =  Max(fabs(b),eps_i);
		A[i][j][k] = A[i][j][k] + b;
	}

    MPI_Barrier(MPI_COMM_WORLD);
    updatematrix();
    MPI_Barrier(MPI_COMM_WORLD);

    waitproccount();
    if (size > 1) {
        MPI_Waitall(f, &req[l], &status[0]);
    }

    MPI_Allreduce(&eps_i, &eps, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    for(i=1; i<=nrow-2; i++)
	for(j=1; j<=N-2; j++)
	for(k=1+(first+i+j+1)%2; k<=N-2; k+=2)
	{
		double b;
		b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
		+A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
		A[i][j][k] = A[i][j][k] + b;
	}

    MPI_Barrier(MPI_COMM_WORLD);
    updatematrix();
    MPI_Barrier(MPI_COMM_WORLD);

    waitproccount();
    if (size > 1) {
        MPI_Waitall(f, &req[l], &status[0]);
    }
}


void verify()
{
	double s,ss = 0;

	s=0.;
	for(i=0; i< (rank == size - 1 ? nrow : nrow - 2); i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		s=s+A[i][j][k]*(i+1+first)*(j+1)*(k+1)/(N*N*N);
	}

    MPI_Reduce(&s, &ss, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("S = %f\n",ss);
    }

}
