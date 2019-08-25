//solver_test.cpp

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>

#include <LocationSolver.hpp>

#define RANGE 8192.0
#define POINTS 64
#define STRENGTH 1000.0

//x	y	z	Strength
//37.65330709517	44.5780989676714	28.8411286473274	2000	

precision_t rand_func(){
	return ((precision_t) (rand()%8337)/4168) - 1.0;
}		

void main(){
	LocationSolver *s = new LocationSolver();
	
	precision_t *xyz_temp;
	
	xyz_t source;
	time_t test;
	time(&test);
	srand(test);
	
	source[0] = RANGE*rand_func();
	source[1] = RANGE*rand_func();
	source[2] = RANGE*rand_func();
	
	for(int i=0;i<POINTS;++i){
		xyz_t temp;
		precision_t stren;
		
		temp[0] = RANGE*rand_func();
		temp[1] = RANGE*rand_func();
		temp[2] = RANGE*rand_func();
		
		stren = STRENGTH/pow(sqrt(
			pow(temp[0]-source[0], 2.0) +
			pow(temp[1]-source[1], 2.0) +
			pow(temp[2]-source[2], 2.0)
		), 2.0);
		
		s->addFrame(temp, stren);
	}
	
	xyz_temp = (precision_t*) s->solve();
	
	printf("result xyz \t: %lf, %lf, %lf\n",
		xyz_temp[0],
		xyz_temp[1],
		xyz_temp[2]
	);
	
	printf("source xyz \t: %lf, %lf, %lf\n",
		source[0],
		source[1],
		source[2]
	);
	
	s->clean();
}
