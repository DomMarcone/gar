//gps_test.c

#include <precision_t.h>
#include <gps_t.h>
#include <gps_convert.h>

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_LENGTH 128

void main(){
	gps_t gps_temp;
	xyz_t xyz_temp;
	char temp[TEMP_LENGTH];
	
	printf("Please enter an input to be converted.\n");
	printf("The input format for gps goes :\n");
	printf("\t41.0, -72.0, 120\n");

	while(1){
		printf(">");
		fgets(temp,TEMP_LENGTH,stdin);
		if(
			sscanf(temp,"%lf %lf %lf\n", 
				&gps_temp.longitude,
				&gps_temp.latitude,
				&gps_temp.altitude
			) != 3
		)return;
				
		to_xyz(&xyz_temp, &gps_temp);
		
		printf("cartesian \t: %f, %f, %f\n",
			xyz_temp[0],
			xyz_temp[1],
			xyz_temp[2]
		);
		
		to_gps(&gps_temp, &xyz_temp);
		
		printf("back to gps \t: %f, %f, %f\n\n",
			gps_temp.longitude,
			gps_temp.latitude,
			gps_temp.altitude
		);
		
	}
}
