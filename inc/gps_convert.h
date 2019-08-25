//gps_convert.h
#pragma once
#ifndef _GPS_CONVERT_H
#define _GPS_CONVERT_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "precision_t.h"
#include "gps_t.h"

//sea level to the Earth's core
#define SEA_LEVEL 6371000.f


void to_gps(gps_t *target, xyz_t *source);

void to_xyz(xyz_t *target, gps_t *source);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPS_CONVERT_H
