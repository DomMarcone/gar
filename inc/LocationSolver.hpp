//LocationSolver.hpp
#pragma once
#ifndef _LOCATION_SOLVER_HPP
#define _LOCATION_SOLVER_HPP

#include <precision_t.h>
#include <gps_t.h>
#include <gps_convert.h>

#include <vector>

typedef struct {
	xyz_t position;
	precision_t strength;
} frame_t;


class LocationSolver {
private :
	std::vector<frame_t> frame;
	
	//used as the cutoff point for calculations
	precision_t noise_floor;
	
	bool solver_ran;//to avoid redundant itterations
	xyz_t result_xyz;
	precision_t error_level;
	
	void sort_frames();
	
	xyz_t *find_center();//finds weighted average
	xyz_t *weighted_center();//finds weighted average
	
	//the function below returns the error rate of a test point
	precision_t test_point(xyz_t source);
	
	void refine_cursor(xyz_t cursor, 
		precision_t amount, 
		size_t remaining_itterations);
		
	void refine_axis(xyz_t cursor, 
		precision_t amount, 
		size_t axis);
	
	size_t max_solver_steps;
	precision_t min_solver_res;
	
public :
	LocationSolver();
	~LocationSolver();
	
	
	void setMaxSolverSteps(size_t s);
	size_t getMaxSolverSteps();
	
	void setMinSolverRes(precision_t s);
	precision_t getMinSolverRes();
	
	precision_t getErrorLevel();
	
	void addFrame( gps_t *gps, precision_t strength);
	void addFrame( xyz_t xyz, precision_t strength);
	void addFrame( precision_t x, precision_t y, precision_t z, precision_t strength);
	
	xyz_t *solve();
	
	//clears points for next itteration
	void clean();
};


#endif //_LOCATION_SOLVER_HPP
