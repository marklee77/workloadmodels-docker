/**
 *  sss - Space Shared Scheduler
 *  Downey model
 */


#ifndef __DOWNEY_H
#define __DOWNEY_H


#include "clock.h"


double downeyspeedup( double A, double sigma, int nodes );
Time downeyexectime( double A, double sigma, Time L, int nodes );
int downeymaxnodes( double A, double sigma );
Time downeyL( double A, double sigma, int nodes, Time exectimeA );


#endif
