#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define  Max(a,b) ((a)>(b)?(a):(b))

#define N 10
double   maxeps = 0.1e-7;
int itmax = 100;
int i,j,k;
double w = 0.5;
double eps;

double A [N][N][N];
int t1,t2, threads;

void relax();
void init();
void verify();

int main(int an, char **as)
{
	int it;
    omp_lock_t lock;
    #ifdef _OPENMP
        t1 = omp_get_wtime();
        omp_init_lock(&lock);
    #endif
    #pragma omp parallel shared (A)
    {
        init();

        //основная вычислительная секция
        try {
            #pragma omp for ordered
            for(it=1; it<=itmax; it++)
            {
                eps = 0.;
                printf("aaa\n");
                printf("%d", omp_test_lock(&lock));
                omp_set_lock(&lock);
                printf("bbb\n");
                relax();
                printf( "it=%4i   eps=%f\n", it,eps);
                omp_unset_lock(&lock);
                if (eps < maxeps) throw "aaa";
            }
        } catch(...) {

        }

    	verify();
    }

    #ifdef _OPENMP
        t2 = omp_get_wtime();
        printf("THREADS: %d TIME: %d\n", threads, t2-t1);
    #endif

    return 0;
}


void init()
{
   #pragma omp for private(k,j)
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
    #ifdef _OPENMP
        threads = omp_get_num_threads();
    #endif

    #pragma omp for private(j,k)
    for(i=1; i<=N-2; i++)
    for(j=1; j<=N-2; j++)
    for(k=1+(i+j)%2; k<=N-2; k+=2)
    {
        double b;
        b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
        +A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
        printf("%d", threads);
        eps =  Max(fabs(b),eps);
        A[i][j][k] = A[i][j][k] + b;
    }

    #pragma omp for private(j,k)
    for(i=1; i<=N-2; i++)
    for(j=1; j<=N-2; j++)
    for(k=1+(i+j+1)%2; k<=N-2; k+=2)
    {
        double b;
        b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
        +A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
        A[i][j][k] = A[i][j][k] + b;
    }
}


void verify()
{
	double s;

	s=0.;
    #pragma omp for private(k,j)
	for(i=0; i<=N-1; i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		s=s+A[i][j][k]*(i+1)*(j+1)*(k+1)/(N*N*N);
	}
	printf("  S = %f\n",s);

}
