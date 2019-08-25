//data_test.cpp

#include <precision_t.h>
#include <gps_t.h>
#include <gps_convert.h>
#include <GpsData.hpp>
#include <RadioData.hpp>
#include <LocationSolver.hpp>

#include <cstdlib>
#include <ctime>

#include <thread>

#define THREADS 4

void thread_function(RadioData *rd, GpsData *gd, int th_count, int th_offset){
	LocationSolver ls;
	FILE *fp;
	
	ls.setNoiseFloor(31.6227766);
	
	for(size_t i=th_offset;i<rd->size();i += th_count){
		frequency_t *f = rd->getFrequency(i);
		
		gps_t *temp_gps_ptr, temp_gps;
		precision_t *temp_xyz;
		time_t timestamp;
		float intensity;
		
		for(int j=0;j<f->size;++j){
			timestamp = f->f[j].t;
			intensity = 1000.0 * pow( 10.0, f->f[j].i/20.0);//*1000.0;
			
			
			if( gd->getStartTime() <= timestamp && 
				timestamp <= gd->getEndTime() )
			{
				xyz_t xyz_temp;
				
				temp_gps_ptr = gd->getLocation(timestamp);
				temp_gps.longitude = temp_gps_ptr->longitude;
				temp_gps.latitude = temp_gps_ptr->latitude;
				temp_gps.altitude = temp_gps_ptr->altitude;
				to_xyz(&xyz_temp, &temp_gps);
				
				/*printf("Adding frame : %f\t%f\t%f\t%f db\n",
					temp_gps->longitude,
					temp_gps->latitude,
					temp_gps->altitude,
					intensity
				);*/
				ls.addFrame(xyz_temp,intensity);
				//ls.addFrame(temp_gps,intensity);
			}
		}
		
		temp_xyz = (precision_t*)ls.solve();
		to_gps(&temp_gps, (xyz_t*)temp_xyz);
		
		if( temp_xyz!=0 && 
			ls.getErrorLevel() < 21.0 &&
			temp_gps.altitude > -10.0 &&
			temp_gps.altitude < 1000.0)//ignore transmitters under ground
			{
			
			printf("Source approximation of %6.3f mhz\t%f %f %fm\n", 
				f->hz/1000000.0,
				temp_gps.longitude,
				temp_gps.latitude,
				temp_gps.altitude
			);
			/*
			printf("Source approximation of %6.3f mhz\t%f,\t%f,\%fm\n", 
				f->hz/1000000.0,
				temp_xyz[0],
				temp_xyz[1],
				temp_xyz[2]
			);*/
		fp = fopen("result.kml","a");
		fprintf(fp,
			"\t\t<Placemark>\n"
			"\t\t\t<name>%.3f Mhz</name>\n"
			"\t\t\t<description>Error level : %.3f</description>\n"
			"\t\t\t<Point>\n"
			"\t\t\t<altitudeMode>absolute</altitudeMode>\n"
			"\t\t\t\t<coordinates>%lf,%lf,%lf</coordinates>\n"
			"\t\t\t</Point>\n"
			"\t\t</Placemark>\n",
			f->hz/1000000.0,
			(float)ls.getErrorLevel(),
			temp_gps.latitude,
			temp_gps.longitude,
			temp_gps.altitude
		);
		fclose(fp);
			
		}
		
		ls.clean();
		//printf("%f : %d measurements\n", f->mhz, f->size);
		
	}
	
}

void main(){
	GpsData gd;
	RadioData rd;
	//LocationSolver ls;
	std::thread *solver_thread[THREADS-1];
	
	FILE *fp = fopen("result.kml","a");
	fprintf(fp,
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
		"\t<Document>\n"
	);
	fclose(fp);
	
	gd.load("testdata_0/gps_data.csv");
	rd.load("testdata_0/radio_spectrum.csv");
	/*
	frequency_t *f = rd.getFrequency((size_t)0);
	for(int i=0;i<f->size;++i){
		printf("* %.3f mhz\t%f.0db\t%us\n",
			f->hz/1000000.0,
			f->f[i].i,
			f->f[i].t
		);
	}
	*/
	printf("Begining solver...\n");
	
	
	for(int i=1;i<THREADS;++i){
		solver_thread[i-1] = new std::thread( thread_function, &rd, &gd, THREADS, i);
	}
	//main thread
	thread_function(&rd, &gd, THREADS, 0);
	
	for(int i=1;i<THREADS;++i){
		solver_thread[i-1]->join();
		delete solver_thread;
	}
	
	fopen("result.kml","a");
	fprintf(fp,
		"\t<Document>\n"
		"</kml>\n"
	);
	fclose(fp);
	
	printf("done!\n");
}