//RadioData.hpp
#pragma once
#ifndef _RADIO_DATA_HPP
#define _RADIO_DATA_HPP

#include <cstdlib>
#include <vector>

typedef struct {
	float i;//intensity
	time_t t;//time
} frequency_frame;

typedef struct{
	float hz;
	size_t size;
	frequency_frame *f;
}frequency_t;

class RadioData{
private :
	std::vector<frequency_t> frequency;

	frequency_t *initFrequency(float hz);
	void addData(float hz, float intensity, time_t time);
	
public :
	RadioData();
	~RadioData();
	
	void load(const char *filename);
	
	frequency_t *getFrequency(float hz);
	frequency_t *getFrequency(size_t i);
	
	size_t size();	
};

#endif //_RADIO_DATA_HPP