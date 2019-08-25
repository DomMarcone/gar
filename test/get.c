#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_LEVEL 0

#define TEMP_LENGTH 128

int tile_count(int level){
	int result=1;
	for(int i=0;i<level;++i)result *= 2;
	return result;
}

char rotate(char in){
	if(in=='a')return 'b';
	if(in=='b')return 'c';
	return 'a';
}

int main(int argc, char *argv[]){
	int level = DEFAULT_LEVEL;
	int tiles;
	char temp[TEMP_LENGTH];
	char server = 'a';
	
	if(argc > 1) level = atoi(argv[1]);
	
	if(level<0){
		printf("Error parsing level %s\n", argv[1]);
	}
	
	tiles = tile_count(level);
	
	for(int y=0;y<tiles;++y){
		for(int x=0;x<tiles;++x){
			//setup the request call
			memset(temp,0,TEMP_LENGTH);
			sprintf(temp,"wget http://%c.tile.openstreetmap.org/%d/%d/%d.png -O %d/%d-%d.png",server,level,x,y,level,x,y);
			
			system(temp);
			
			server = rotate(server);
		}
	}
	
	
	return 0;
}