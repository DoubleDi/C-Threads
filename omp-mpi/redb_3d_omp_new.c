#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define  Max(a,b) ((a)>(b)?(a):(b))
#define  Min(a,b) ((a)>(b)?(b):(a))

#define N 100
double   maxeps = 0.1e-7;
int itmax = 100;
int i,j,k;
double w = 0.5;
double eps;

double A [N][N][N];
int t1,t2, threads;

int iam, limit, numt;
int *isync;

void relax();
void init();
void verify();

int main(int an, char **as)
{
	int it;
    isync = (int *)malloc(omp_get_max_threads() * sizeof(*isync));
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

    printf("THREADS: %d TIME: %d\n", threads, t2-t1);

    return 0;
}


void init()
{
   // pragma omp parallel for private(k,j)
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
        #ifdef _OPENMP
            threads = omp_get_num_threads();
        #endif

    #pragma omp parallel private(iam,numt,limit)
    {
        iam = omp_get_thread_num();
        numt = omp_get_num_threads();
        limit = Min(numt-1, N-2);
        isync[iam] = 0;
        #pragma omp barrier
        for(i=1; i<=N-2; i++) {
            if ((iam > 0) && (iam <= limit)) {
                for (;isync[iam-1]==0;) {
                    #pragma omp flush (isync)
                }
            }
            #pragma omp schedule(static) nowait
            for(j=1; j<=N-2; j++) {
                for(k=1+(i+j)%2; k<=N-2; k+=2)
                {
                    double b;
                    b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
                        +A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
                        eps =  Max(fabs(b),eps);
                        // printf("i %d j %d k %d b %lf\n", i,j,k,b);
                        A[i][j][k] = A[i][j][k] + b;
                    }
            }
            if (iam < limit) {
                for (;isync[iam]==1;) {
                    #pragma omp flush (isync)
                }
                isync[iam] = 1;
                #pragma omp flush (isync)
            }
        }

        #pragma omp barrier
        for(i=1; i<=N-2; i++) {
            if ((iam > 0) && (iam <= limit)) {
                for (;isync[iam-1]==0;) {
                    #pragma omp flush (isync)
                }
            }
            #pragma omp schedule(static) nowait
            for(j=1; j<=N-2; j++) {
                for(k=1+(i+j)%2; k<=N-2; k+=2)
                {
                    double b;
                    b = w*((A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]
                        +A[i][j][k-1]+A[i][j][k+1] )/6. - A[i][j][k]);
                        A[i][j][k] = A[i][j][k] + b;
                    }
            }
            if (iam < limit) {
                for (;isync[iam]==1;) {
                    #pragma omp flush (isync)
                }
                isync[iam] = 1;
                #pragma omp flush (isync)
            }
        }
    }
}


void verify()
{
	double s;

	s=0.;
    // pragma omp parallel for reduction (+:s) private(k,j)
	for(i=0; i<=N-1; i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		s=s+A[i][j][k]*(i+1)*(j+1)*(k+1)/(N*N*N);
	}
	printf("  S = %f\n",s);

}
