#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )  {
	int N, K, T, O;
	char *n = argv[1];
	char *k = argv[2];
	N = atoi(n); // char * to int
	K = atoi(k); // char * to int
	O = N%K; 
	if (O != 0){
		T = N/K + 1;
	}
	else{
		T = N/K;
	}
	printf("Stairs: %i \n", N);
	printf("Stairs per step: %i \n", K);
	printf("Total steps: %i \n", T);
	return 0;
}