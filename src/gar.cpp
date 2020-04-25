//gar.cpp

#include <precision_t.h>
#include <gps_t.h>
#include <gps_convert.h>
#include <GpsData.hpp>
#include <RadioData.hpp>
#include <LocationSolverInterface.hpp>
#include <LocationSolverBST.hpp>

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>

#include <thread>

#define DEFAULT_NOISE_FLOOR -40
#define DEFAULT_THREADS 4

const char *  DEFAULT_FILE_OUT = "result.kml";

void thread_function(
	RadioData *rd, 
	GpsData *gd, 
	const char *outfile, 
	precision_t noise_floor, 
	int th_count, 
	int th_offset){
		
	LocationSolverInterface *ls = new LocationSolverBST();
	FILE *fp;
		
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
				timestamp <= gd->getEndTime() &&
				intensity >= noise_floor )
			{
				xyz_t xyz_temp;
				
				temp_gps_ptr = gd->getLocation(timestamp);
				temp_gps.longitude = temp_gps_ptr->longitude;
				temp_gps.latitude = temp_gps_ptr->latitude;
				temp_gps.altitude = temp_gps_ptr->altitude;
				to_xyz(xyz_temp, &temp_gps);
				
				ls->addFrame(xyz_temp,intensity);
				//ls.addFrame(temp_gps,intensity);
			}
		}
		
		temp_xyz = (precision_t*)ls->solve();
		/*
		if( temp_xyz!=0 && 
			ls.getErrorLevel() < 21.0 &&
			temp_gps.altitude > -10.0 &&
			temp_gps.altitude < 1000.0)//ignore transmitters under ground
			{
			*/
		if( temp_xyz!=0 ){
			to_gps(&temp_gps, temp_xyz);
			
			printf("Source approximation of %6.3f mhz\t%f %f %fm\n", 
				f->hz/1000000.0,
				temp_gps.longitude,
				temp_gps.latitude,
				temp_gps.altitude
			);
				
			fp = fopen(outfile,"a");
			fprintf(fp,
				"\t\t<Placemark>\n"
				"\t\t\t<name>%.3f Mhz</name>\n"
				"\t\t\t<description>Frequency %.0f<br/>Error level : %.3f</description>\n"
				"\t\t\t<Point>\n"
				"\t\t\t<altitudeMode>absolute</altitudeMode>\n"
				"\t\t\t\t<coordinates>%lf,%lf,%lf</coordinates>\n"
				"\t\t\t</Point>\n"
				"\t\t</Placemark>\n",
				f->hz/1000000.0,
				f->hz,
				(float)ls->getErrorLevel(),
				temp_gps.latitude,
				temp_gps.longitude,
				temp_gps.altitude
			);
			fclose(fp);
				
		}
		
		ls->clean();
	}
	
}


void print_desc(){
	printf(
		"Tries to determine the source of a broadcast using a gps log .csv file\n"
		"and an rtl_power genrated .csv file.\n\n"
	);
}

void print_usage(){
	printf(
		"usage: gar [options] -r <file from rtl_power>.csv -g <file from gps>.csv\n"
	);
}

void print_options(){
	printf(
	"options : \n"
	"  -g\tthe input .csv from gps logging software\n"
	"  -r\tthe input .csv from rtl_power\n"
	"  -o\tname of the output .kml file\n"
	"  -n\tnoise floor - a cutoff for the solver.\n"
	"  -t\tnumber of threads\n"
	"  -h\tdisplay this help screen\n"
	);
}


int main(int argc, char *argv[]){
	GpsData gd;
	RadioData rd;
	//std::thread *solver_thread[THREADS-1];
	std::thread **solver_thread;
	
	bool solve = false;
	
	int thread_count = DEFAULT_THREADS;
	const char *file_out = DEFAULT_FILE_OUT;
	precision_t noise_floor = DEFAULT_NOISE_FLOOR;
	char *file_gps=0, *file_radio=0;
	
	for(int i=1; i<argc; ++i){
		
		if(!strcmp(argv[i], "-h") ||
			!strcmp(argv[i], "--help")){
			print_desc();
			print_usage();
			print_options();
			exit(0);
		}
		
		if(!strcmp(argv[i], "-g") && i<argc-1){
			i++;
			file_gps = argv[i];
			continue;
		}
		
		if(!strcmp(argv[i], "-r") && i<argc-1){
			i++;
			file_radio = argv[i];
			continue;
		}	
		
		if(!strcmp(argv[i], "-o") && i<argc-1){
			i++;
			file_out = argv[i];
			continue;
		}
		
		if(!strcmp(argv[i], "-n") && i<argc-1 ){
			i++;
			noise_floor = (precision_t)atof(argv[i]);
			continue;
		}
		
		if(!strcmp(argv[i], "-t") && i<argc-1 ){
			i++;
			thread_count = (precision_t)atoi(argv[i]);
			//so, we don't run into problems later
			if(thread_count==0)thread_count = 1;
			continue;
		}
		
	}
	
	if(file_gps && file_radio)solve = true;
	
	if(solve){
		FILE *fp = fopen(file_out,"a");
		fprintf(fp,
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
			"\t<Document>\n"
		);
		fclose(fp);
		
		gd.load(file_gps);
		rd.load(file_radio);
		
		printf(
			"\n"
			"Solver Parameters :\n"
			"  gps file            : %s\n"
			"  radio file          : %s\n"
			"  output file         : %s\n"
			"  threads             : %d\n"
			"  noise floor cuttoff : %f\n"
			"\n",
			file_gps, file_radio, file_out,
			thread_count, noise_floor
		);
		
		printf("Begining solver...\n");
		
		//convert to intensity
		noise_floor = 1000.0 * pow( 10.0, noise_floor/20.0);
		
		solver_thread = (std::thread**)malloc(sizeof(std::thread*) * (thread_count-1));
		
		for(int i=1;i<thread_count;++i){
			solver_thread[i-1] = 
				new std::thread( thread_function, 
					&rd, &gd, 
					file_out, 
					noise_floor, 
					thread_count, i);
		}
		//main thread
		thread_function(
			&rd, &gd, 
			file_out, 
			noise_floor, 
			thread_count, 0);
		
		//join previously generated threads...
		for(int i=1;i<thread_count;++i){
			solver_thread[i-1]->join();
		}
		
		
		fp = fopen(file_out,"a");
		fprintf(fp,
			"\t</Document>\n"
			"</kml>\n"
		);
		fclose(fp);
		
		printf("done!\n");
		
		for(int i=1;i<thread_count;++i){
			delete solver_thread[i-1];
		}
		
		free(solver_thread);
	} else {
		print_usage();
		printf(
			"Try `gar --help' for more info.\n"
		);
	}

	return 0;
}
