//LocationSolverInterface.cpp

#include <cmath>
#include <stdio.h>

#include <precision_t.h>
#include <gps_t.h>
#include <gps_convert.h>
#include <LocationSolverInterface.hpp>

//LocationSolverInterface::LocationSolverInterface() {}
//LocationSolverInterface::~LocationSolverInterface() {}

void LocationSolverInterface::addFrame(gps_t* gps, precision_t strength) {}
void LocationSolverInterface::addFrame(xyz_t xyz, precision_t strength) {}
void LocationSolverInterface::addFrame(precision_t x, precision_t y, precision_t z, precision_t strength) {}

xyz_t* LocationSolverInterface::solve() { return 0; }

precision_t LocationSolverInterface::getErrorLevel() { return 0; }

void LocationSolverInterface::clean() {}
