//LocationSolverInterface.hpp
#pragma once
#ifndef _LOCATION_SOLVER_INTERFACE_HPP
#define _LOCATION_SOLVER_INTERFACE_HPP

#include <precision_t.h>
#include <gps_t.h>
#include <gps_convert.h>


class LocationSolverInterface {
private :
	
public :
	//LocationSolverInterface();
	//~LocationSolverInterface();
	
	virtual void addFrame( gps_t *gps, precision_t strength);
	virtual void addFrame( xyz_t xyz, precision_t strength);
	virtual void addFrame( precision_t x, precision_t y, precision_t z, precision_t strength);
	
	virtual xyz_t *solve();
	
	virtual precision_t getErrorLevel();

	//clears points for next itteration
	virtual void clean();
};


#endif //_LOCATION_SOLVER_INTERFACE_HPP
