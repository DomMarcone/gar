//GpsData.cpp

#include <GpsData.hpp>

#include <precision_t.h>
#include <gps_t.h>

#include <cstdlib>
#include <ctime>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_LINE_LENGTH 192

GpsData::GpsData(){}
GpsData::~GpsData(){
	frame.erase(frame.begin(),frame.end());
}

int load_frame(gps_frame *gf, char * csv_line){
	tm temp_time;
	int scan_result;
	float longitude, latitude, altitude;
	
	//make it 0
	memset(&temp_time,0,sizeof(tm));
	
	scan_result = sscanf(
		(const char*)csv_line,
		"%d-%d-%d %d:%d:%d +%*d;%f;%f;%f;%*s\n",
		&temp_time.tm_year,
		&temp_time.tm_mon,
		&temp_time.tm_mday,
		&temp_time.tm_hour,
		&temp_time.tm_min,
		&temp_time.tm_sec,
		&longitude,
		&latitude,
		&altitude
		);
	
	if(scan_result != 9){
		return 0;
	}
	
	temp_time.tm_year -= 1900;
	temp_time.tm_mon -= 1;
	
	gf->g.longitude = longitude;
	gf->g.latitude = latitude;
	gf->g.altitude = altitude;
	
	gf->t = mktime(&temp_time);
		
	return 1;
}

void GpsData::load(const char *filename){
	FILE *fp;
	char temp_line[TEMP_LINE_LENGTH];
	
	//make sure the extension is .csv
	if(
		memcmp(filename+strlen(filename)-4,".csv",4) != 0
	){
		printf("GpsData : Warning! GPS data file : %s doesn't have the correct '.csv' extension!\n",filename);
		//return;//doesn't have to return, could be a simple mistake
	}
	
	fp = fopen(filename,"r");

	if(!fp){
		printf("GpsData : Error! Couldn't open %s\n", filename);
		return;
	}
	
	printf("GpsData \t: loading %-60s",filename);
	
	while(!feof(fp)){
		gps_frame temp_gps;
		fgets(temp_line,TEMP_LINE_LENGTH,fp);
		
		if(	load_frame(&temp_gps,temp_line) ){//do we have a valid line?
			frame.push_back(temp_gps);//if so, add it
		}
	}
	
	/*
	for(int i=0;i<frame.size();++i){
		printf("Frame %d : \t%f \t%f \t%f \t%ul\n", i,
			frame[i].g.longitude,
			frame[i].g.latitude,
			frame[i].g.altitude,
			frame[i].t
		);
	}
	*/
	
	printf("done\n");
	
	fclose(fp);
}


time_t GpsData::getStartTime(){
	return frame[0].t;
}

time_t GpsData::getEndTime(){
	return frame[frame.size()-1].t;
};

gps_t *GpsData::getLocation(time_t t){
	//binary search time
	gps_t interpolated;
	float weight, range;
	
	//int result;
	int left = 0, right = frame.size()-1;
	
	if(t <= getStartTime())return &frame[0].g;
	if(t >= getEndTime())return &frame[frame.size()-1].g;
	
	while(left<right){
		int middle = (left+right)/2;
		if( frame[middle].t < t ){
			left = middle + 1;
		} else /*( frame[middle].t > t )*/{
			right = middle - 1;
		}
	}
	
	if(left==right)right=left+1;
	
	//substituting binary search with working function
	//while( difftime(frame[result].t, t) < 0.0)pivot++;
	
	//good results at this point
	
	range = (float)difftime(frame[left].t, frame[right].t);
	weight = (float)difftime(t, frame[left].t);
	
	interpolated.longitude = (frame[left].g.longitude*weight) + 
		(frame[right].g.longitude*(1.0-weight));
	interpolated.latitude = (frame[left].g.latitude*weight) + 
		(frame[right].g.latitude*(1.0-weight));
	interpolated.altitude = (frame[left].g.altitude*weight) + 
		(frame[right].g.altitude*(1.0-weight));
	
	/*
	printf("returning \t%lf\t%lf\t%lf\n",
		interpolated.longitude,
		interpolated.latitude,
		interpolated.altitude
	);*/
	
	return &interpolated;
}
