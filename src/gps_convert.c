//gps_convert.c

#include "gps_convert.h"
#include "precision_t.h"

#define _USE_MATH_DEFINES

#include <math.h>

//helper functions
precision_t to_radians(precision_t in){
	return (M_PI*in)/180.0;
}

precision_t to_degrees(precision_t in){
	return (180.0*in)/M_PI;
}


void to_gps(gps_t *target, xyz_t source){
	precision_t plane_length = sqrt(source[0]*source[0] + source[1]*source[1]);
	precision_t magnitude = sqrt(plane_length*plane_length + source[2]*source[2]);
	
	target->latitude = to_degrees(atan2( (double)source[2], (double)plane_length ));
	target->longitude = to_degrees( atan2((double)source[1], (double)source[0]));
	target->altitude = magnitude - SEA_LEVEL;
}


void to_xyz(xyz_t target, gps_t *source){
	precision_t to_core = SEA_LEVEL + source->altitude;
	precision_t radius_at_latitude = to_core * cos(to_radians(source->latitude));
	
	
	target[0] = radius_at_latitude * cos(to_radians(source->longitude));
	target[1] = radius_at_latitude * sin(to_radians(source->longitude));
	target[2] = to_core * sin(to_radians(source->latitude));
}
