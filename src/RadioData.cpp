//RadioData.cpp

#include <cstdlib>
#include <ctime>
#include <vector>


#include <RadioData.hpp>


#define TEMP_LINE_LENGTH 4096


RadioData::RadioData(){}

RadioData::~RadioData(){
	for(size_t i=0;i<frequency.size();++i){
		free(frequency[i].f);
	}
	
	frequency.erase(frequency.begin(),frequency.end());
}

void RadioData::load(const char * filename){
	FILE *fp;
	char temp_line[TEMP_LINE_LENGTH];
	
	//make sure the extension is .csv
	if(
		memcmp(filename+strlen(filename)-4,".csv",4) != 0
	){
		printf("RadioData : Warning! rtl_power output %s doesn't have the correct '.csv' extension!\n",filename);
		//return;//doesn't have to return, could be a simple mistake
	}
	
	fp = fopen(filename,"r");

	if(!fp){
		printf("RadioData : Error! Couldn't open %s\n", filename);
		return;
	}
	
	printf("RadioData \t: loading %-60s", filename);
	
	while(!feof(fp)){
		int scan_result;
		int start_freq, end_freq; 
		float freq_step, current_freq;
		tm temp_time;
		time_t timestamp;
		
		fgets(temp_line,TEMP_LINE_LENGTH,fp);
		
		memset(&temp_time,0,sizeof(tm));
		
		scan_result = sscanf(
			(const char*)temp_line,
			"%d-%d-%d, %d:%d:%d, %d, %d, %f, %*s\n",
			&temp_time.tm_year,
			&temp_time.tm_mon,
			&temp_time.tm_mday,
			&temp_time.tm_hour,
			&temp_time.tm_min,
			&temp_time.tm_sec,
			&start_freq,
			&end_freq,
			&freq_step
			);
			
		if(scan_result == 9){
			size_t current_position = 0;
			for(int commas_left = 6;
				commas_left>0 &&
					current_position<TEMP_LINE_LENGTH;
				++current_position)if(temp_line[current_position]==','){
					commas_left--;
				}
			current_position++;
				
			temp_time.tm_year -= 1900;
			temp_time.tm_mon -= 1;
			
			timestamp = mktime(&temp_time);
			
			current_freq = (float)start_freq;
			
			//printf("Start %d, end %d, step %f\n",start_freq, end_freq, freq_step);
			
			while(current_freq <= end_freq){
				float intensity = atof((const char *)&temp_line[current_position]);

				//if(((int)current_freq)%100 == 0){
				addData(current_freq,intensity,timestamp);
				//}
				
				//advance the line and frequency to the next position
				current_position++;
				while(temp_line[current_position]!=',' &&
					current_position<TEMP_LINE_LENGTH)
				{
					current_position++;
				}
				current_position++;
				
				current_freq += freq_step;
			}
			
		}
		
	}
	
	printf("done\n");
	
	fclose(fp);
}


frequency_t *RadioData::initFrequency(float hz){
	frequency_t temp;
	temp.f = (frequency_frame*)malloc(sizeof(frequency_frame));//get it ready for allocation
	temp.size = 0;
	temp.hz = hz;
	
	frequency.push_back(temp);
	
	return &frequency[frequency.size()-1];
}


frequency_t *RadioData::getFrequency(float hz){
	/*
	for(size_t i=0;i<frequency.size();++i){
		if(frequency[i].hz == hz){
			//printf("Found %fmhz\n",mhz);
			return &frequency[i];
		}
	}
	
	/*/
	
	//Binary search time.
	int left=0, right=frequency.size()-1;
	
	while(left<=right){
		int middle = (left+right)/2;
		if(frequency[middle].hz < hz){
			left = middle + 1;
		} else if (frequency[middle].hz > hz) {
			right = middle - 1;
		} else {
			return &frequency[middle];
		}
	}
	//*/
	//printf("Initializing %fmhz\n", mhz);
	
	return initFrequency(hz);
}

frequency_t *RadioData::getFrequency(size_t i){
	return &frequency[i];
}

size_t RadioData::size(){return frequency.size();}

void RadioData::addData(float hz, float intensity, time_t time){
	frequency_t *current_freq = getFrequency(hz);
	frequency_frame *current_frame;
	current_freq->size++;
	
	current_freq->f = (frequency_frame*)realloc((void*)current_freq->f, sizeof(frequency_frame) * current_freq->size);
	//printf("Adding data %fmhz\t%f\t%u\n", hz, intensity, time);
	current_frame = current_freq->f;
	
	current_frame += current_freq->size-1;
	
	current_frame->t = time;
	current_frame->i = intensity;
}

