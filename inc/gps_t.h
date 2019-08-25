//gps_t.h
#pragma once
#ifndef _GPS_T_H
#define _GPS_T_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//gps coordinate type
typedef struct gps_t{
	precision_t longitude;
	precision_t latitude;
	precision_t altitude;
}gps_t;

//cartesian coordinate type
typedef precision_t xyz_t[3];

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPS_T_H
