#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )  {
	int N, D, T;
	char *n = argv[1];
	char *d = argv[2];
	T = 0;
	N = atoi(n); // char * to int
	D = atoi(d); // char * to int 
	for (int i = 0; i < N+1; i++){T = T + i * D;}
	printf("The total number of spans: %i \n", N);
	printf("The distance between the spans: %i \n", D);
	printf("Total lenght of cables: %i \n", T);
	return 0;
}
