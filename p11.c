#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <openacc.h>

#define MAX_TEMP_ERROR 0.00001
#define ROWS 5000
#define COLUMNS 5000

float A[ROWS][COLUMNS],A_new[ROWS][COLUMNS];

int main(){
	float dt=876788;
	int max_iterations=1000;
	int iterations=0;
	int i,j;
	struct timeval tim;
	double t1, t2;

	memset(A, 0,((long)ROWS) *COLUMNS * sizeof(float));

	gettimeofday(&tim, NULL);

	t1=tim.tv_sec+(tim.tv_usec/1000000.0);

	while(dt>MAX_TEMP_ERROR && iterations<=max_iterations){
		#pragma acc kernels
		for(i=1;i<ROWS-1;i++){
			for(j=1;j<COLUMNS-1;j++){
				A_new[i][j] = 0.25f*(A[i+1][j]+A[i-1][j]+A[i][j+1]+A[i][j-1]);
			}
		}

		dt = 0.0;

		#pragma acc kernels
		for(i=1;i<ROWS-1;i++) {
			for(j=1;j<COLUMNS-1;j++) {
				dt = fmax(fabs(A_new[i][j]-A[i][j]),dt);
				A[i][j]=A_new[i][j];
			}
		}
		iterations++;
	}

	gettimeofday(&tim, NULL);

	t2=tim.tv_sec+(tim.tv_usec/1000000.0);

	printf("%.6lf seconds with OpenACC \n", t2-t1);
	return 0;
}
