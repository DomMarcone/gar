//LocationSolver.cpp

#include <cmath>
#include <stdio.h>

#include <precision_t.h>
#include <gps_t.h>
#include <gps_convert.h>
#include <LocationSolver.hpp>

LocationSolver::LocationSolver(){
	solver_ran = false;
	noise_floor = 0.0;
	result_xyz[0] = 0.0;
	result_xyz[1] = 0.0;
	result_xyz[2] = 0.0;
	max_solver_steps = 128;
	min_solver_res = 0.001;
	error_level = 0.0;
	//printf("LocationSolver : constructor called.\n");
}

LocationSolver::~LocationSolver(){
	clean();
}

void LocationSolver::addFrame(precision_t x, precision_t y, precision_t z, precision_t strength){
	frame_t temp;
	temp.position[0] = x;
	temp.position[1] = y;
	temp.position[2] = z;
	temp.strength = strength;
	frame.push_back(temp);
}

void LocationSolver::addFrame(xyz_t xyz, precision_t strength){
	frame_t temp;
	temp.position[0] = xyz[0];
	temp.position[1] = xyz[1];
	temp.position[2] = xyz[2];
	temp.strength = strength;
	frame.push_back(temp);
}

void LocationSolver::addFrame(gps_t *gps, precision_t strength){
	frame_t temp;
	xyz_t xyz;
	to_xyz(xyz, gps);
	temp.position[0] = xyz[0];
	temp.position[1] = xyz[1];
	temp.position[2] = xyz[2];
	temp.strength = strength;
	frame.push_back(temp);
}


void LocationSolver::setMaxSolverSteps(size_t s){max_solver_steps = s;}
size_t LocationSolver::getMaxSolverSteps(){return max_solver_steps;}

void LocationSolver::setMinSolverRes(precision_t s){min_solver_res = s;}
precision_t LocationSolver::getMinSolverRes(){return min_solver_res;}

precision_t get_dist(xyz_t a, xyz_t b){
	precision_t x, y, z;
	x = a[0] - b[0];
	y = a[1] - b[1];
	z = a[2] - b[2];
	
	return sqrt(x*x + y*y + z*z);
}


size_t get_index_of_furthest_point(std::vector<frame_t>frm, xyz_t source){
	precision_t max_dist;
	size_t max_index = 0;
	
	//sanity check
	if(frm.size()<=0)return 0;
	
	max_dist = get_dist(frm[max_index].position, source);
	
	for(int i=1;i<frm.size();++i){
		precision_t temp_dist = get_dist(frm[i].position, source);
		if(temp_dist > max_dist){
			max_dist = temp_dist;
			max_index = i;
		}
	}
	return max_index;
}

size_t get_index_of_closest_point(std::vector<frame_t>frm, xyz_t source){
	precision_t min_dist;
	size_t min_index = 0;
	
	//sanity check
	if(frm.size()<=0)return 0;
	
	min_dist = get_dist(frm[min_index].position, source);
	
	for(int i=1;i<frm.size();++i){
		precision_t temp_dist = get_dist(frm[i].position, source);
		if(temp_dist < min_dist){
			min_dist = temp_dist;
			min_index = i;
		}
	}
	return min_index;
}


precision_t calculate_expected_value(precision_t distance, precision_t strength){	
	return strength/(distance*distance);
}


precision_t LocationSolver::test_point( xyz_t source ){
	precision_t result = 0.0;
	precision_t strength, max = 0.0;
	static precision_t inverse_res = 1.0/min_solver_res;
	size_t closest;
	
	//we'll need frames to have at least one value (and be sorted)
	if(frame.size()<=0)return 0.0;
	
	//closest = get_index_of_closest_point(frame, source);
	closest = 0;
	
	//strength = pow(get_dist(frame[closest].position, source), 2.0);
	strength = frame[closest].strength * pow(get_dist(frame[closest].position, source), 2.0);
	
	for(int i=0;i<frame.size();++i)
		max = max > frame[i].strength ? max : frame[i].strength;
	
	//printf("Expected strength : %f \t%d\n", strength, closest);
	
	for(int i=0;i<frame.size();++i){
		precision_t dist = get_dist( frame[i].position, source );
		result += pow(
			(sqrt(strength/frame[i].strength) - dist)*inverse_res
		, 2.0);
	}
	//return result/frame.size();
	return result/frame.size();
}


void LocationSolver::refine_axis(xyz_t cursor, 
	precision_t amount, 
	size_t axis){
	
	precision_t test_error;
	precision_t current_error = test_point(cursor);
	
	cursor[axis]+=amount;
	
	test_error = test_point(cursor);
	if(test_error < current_error)return;//found a better solution
	
	//didn't so, double back
	cursor[axis]-=amount*2.0;
	
	test_error = test_point(cursor);
	if(test_error < current_error)return;//found a better solution
	
	//didn't so go back to the start
	cursor[axis]+=amount;
}


void LocationSolver::refine_cursor(xyz_t cursor, 
	precision_t amount, 
	size_t remaining_itterations){
	
	if(remaining_itterations<=0 || amount<min_solver_res)return;
	

	refine_axis(cursor, amount, 0);
	refine_axis(cursor, amount, 1);
	refine_axis(cursor, amount, 2);
	
	//printf("Error at cursor %f\n",test_point(cursor));
	
	refine_cursor(cursor, amount/2.0, remaining_itterations-1);
}


xyz_t *LocationSolver::solve(){
	if(!solver_ran){
		xyz_t cursor;
		precision_t *temp;
				
		//initial sanity check
		if(frame.size() < 4){
			//printf("LocationSolver : ERROR : There aren't enough frames to solve! Need 4; found %d",
			//frame.size());
			return 0;
		}
		
		sort_frames();
		
		temp = (precision_t *)weighted_center();
		//temp = (precision_t *)find_center();
		
		cursor[0] = temp[0];
		cursor[1] = temp[1];
		cursor[2] = temp[2];
		/*
		printf("Weighted Center \t: %f, %f, %f\n",
			cursor[0],
			cursor[1],
			cursor[2]
		);
		
		printf("Error at cursor \t: %f\n", test_point(cursor));
		*/
		for(int i=0;i<max_solver_steps;++i){
			refine_cursor(cursor,
					get_dist(
						//frame[get_index_of_furthest_point(frame,cursor)].position, 
						frame[0].position, 
						cursor),
				max_solver_steps);
			
			//if(test_point(cursor)<=min_solver_error)break;
		}
		
		
		//printf("Error at cursor \t: %f\n", test_point(cursor));
		
		result_xyz[0] = cursor[0];
		result_xyz[1] = cursor[1];
		result_xyz[2] = cursor[2];
		
		error_level = log10(test_point(cursor));
		
		solver_ran = true;
	}
	return &result_xyz;
}


precision_t LocationSolver::getErrorLevel(){return error_level; }


void LocationSolver::clean(){
	
	frame.erase(frame.begin(), frame.end());
	
	solver_ran = false;
}


//helper functions for sorting...
void merge(std::vector<frame_t> *a, size_t left, size_t right){
	size_t i, j, k;
	size_t middle = (right+left)/2;
	size_t left_size = middle-left;
	size_t right_size = right-middle;
	std::vector<frame_t> *left_array, *right_array;

	left_array = new std::vector<frame_t>;
	right_array = new std::vector<frame_t>;

	//copy arrays...
	for(i=0;i<left_size;++i)left_array->push_back( a->at(i+left) );
	for(j=0;j<right_size;++j)right_array->push_back( a->at(j+middle) );

	i=0;j=0;k=left;

	while( i<left_size && j<right_size ){
    if(left_array->at(i).strength >= right_array->at(j).strength){
		a->at(k).position[0] = left_array->at(i).position[0];
		a->at(k).position[1] = left_array->at(i).position[1];
		a->at(k).position[2] = left_array->at(i).position[2];
		a->at(k).strength = left_array->at(i).strength;
		k++; i++;
    } else {
		a->at(k).position[0] = right_array->at(j).position[0];
		a->at(k).position[1] = right_array->at(j).position[1];
		a->at(k).position[2] = right_array->at(j).position[2];
		a->at(k).strength = right_array->at(j).strength;
		k++; j++;
    }
  }

	while( i<left_size ){
		a->at(k).position[0] = left_array->at(i).position[0];
		a->at(k).position[1] = left_array->at(i).position[1];
		a->at(k).position[2] = left_array->at(i).position[2];
		a->at(k).strength = left_array->at(i).strength;
		k++; i++;
	}

	while( j<right_size ){
		a->at(k).position[0] = right_array->at(j).position[0];
		a->at(k).position[1] = right_array->at(j).position[1];
		a->at(k).position[2] = right_array->at(j).position[2];
		a->at(k).strength = right_array->at(j).strength;
		k++; j++;
	}

	delete left_array;
	delete right_array;
}


void merge_sort(std::vector<frame_t> *a, size_t left, size_t right){
	if( right - left > 1 )
	{
		size_t middle = (right + left)/2;
		
		merge_sort(a, left, middle);
		merge_sort(a, middle, right);

		merge(a, left, right);
	}
}

void LocationSolver::sort_frames(){
	merge_sort(&frame,0,frame.size());
	/*
	printf("After Sort :\n");
	for(int i=0;i<frame.size();++i){
		printf("%4d : %5f, %5f, %5f, %5f\n", i,
			frame[i].position[0],
			frame[i].position[1],
			frame[i].position[2],
			frame[i].strength
			);
	}*/
}


xyz_t *LocationSolver::weighted_center(){
	xyz_t result_vector;
	precision_t strength_sum = 0;
	
	result_vector[0] = 0;
	result_vector[1] = 0;
	result_vector[2] = 0;
	
	for(int i=0;i<frame.size();++i){
		strength_sum += frame[i].strength;
	}
	
	for(int i=0;i<frame.size();++i){
		precision_t weight = frame[i].strength / strength_sum;
		
		result_vector[0] += frame[i].position[0] * weight;
		result_vector[1] += frame[i].position[1] * weight;
		result_vector[2] += frame[i].position[2] * weight;
	}
	
	return &result_vector;
}


xyz_t *LocationSolver::find_center(){
	xyz_t result_vector;
	size_t size = frame.size();
	
	result_vector[0] = 0;
	result_vector[1] = 0;
	result_vector[2] = 0;
	
	for(int i=0;i<frame.size();++i){
		
		result_vector[0] += frame[i].position[0]/size;
		result_vector[1] += frame[i].position[1]/size;
		result_vector[2] += frame[i].position[2]/size;
	}
	
	return &result_vector;
}
