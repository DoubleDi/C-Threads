#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define  Max(a,b) ((a)>(b)?(a):(b))

#define N 100
double   maxeps = 0.1e-7;
int itmax = 100;
int i,j,k;
double w = 0.5;
double eps;

double A [N][N][N];
int threads;

void relax();
void init();
void verify();

int main(int an, char **as)
{
	int it;
    double t1, t2;
    init();

    #ifdef _OPENMP
        t1 = omp_get_wtime();
    #endif

    //основная вычислительная секция
	for(it=1; it<=itmax; it++)
	{
        eps = 0.;
		relax();
		printf( "it=%4i   eps=%f\n", it,eps);
		if (eps < maxeps) break;
	}

    #ifdef _OPENMP
        t2 = omp_get_wtime();
    #endif

	verify();

    printf("THREADS: %d TIME: %lf\n", threads, t2-t1);

    return 0;
}


void init()
{
    #pragma omp parallel for private(k,j)
	for(i=0; i<=N-1; i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		if(i==0 || i==N-1 || j==0 || j==N-1 || k==0 || k==N-1)     A[i][j][k]= 0.;
		else A[i][j][k]= ( 4. + i + j + k) ;
	}
}


void relax()
{
    double eps_i = eps;
    #pragma omp parallel private(k) firstprivate(eps_i)
    {
        int rank, size;
        #ifdef _OPENMP
            threads = omp_get_num_threads();
            rank = omp_get_thread_num();
            size = omp_get_num_threads();
        #endif
        for(int newi=1; newi<=N+size-1; newi++) {
            int i = newi - rank;
            #pragma omp for
            for(j=1; j<=N-2; j++)
            for(k=1+(i+j)%2; k<=N-2; k+=2)
            {
                double b;
                if (i >= 1 && i < N-1) {
                    b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
                    +A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
                    eps_i =  Max(fabs(b),eps_i);
                    A[i][j][k] = A[i][j][k] + b;
                }
            }
        }
        #pragma omp critical
        eps = Max(eps_i, eps);

        #pragma omp barrier

        for(int newi=1; newi<=N+size-1; newi++) {
            int i = newi - rank;
            #pragma omp for
            for(j=1; j<=N-2; j++)
            for(k=1+(i+j+1)%2; k<=N-2; k+=2)
            {
                double b;
                if (i >= 1 &&  i < N-1) {
                    b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
                    +A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
                    A[i][j][k] = A[i][j][k] + b;
                }
            }
        }
    }

}


void verify()
{
	double s;

	s=0.;
    #pragma omp parallel for reduction (+:s) private(k,j)
	for(i=0; i<=N-1; i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		s=s+A[i][j][k]*(i+1)*(j+1)*(k+1)/(N*N*N);
	}
	printf("S = %f\n",s);

}
