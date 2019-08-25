//GpsData.hpp
#pragma once
#ifndef _GPS_DATA_HPP
#define _GPS_DATA_HPP

#include <cstdlib>
#include <ctime>
#include <vector>

#include <precision_t.h>
#include <gps_t.h>


typedef struct {
	gps_t g;
	time_t t;
}gps_frame;

class GpsData {
private :
	std::vector<gps_frame> frame;
	
	time_t start, end;
	
public :
	GpsData();
	~GpsData();
	
	void load(const char * filename);
		
	time_t getStartTime();
	time_t getEndTime();
	
	gps_t *getLocation(time_t t);
};

#endif //_GPS_DATA_HPP