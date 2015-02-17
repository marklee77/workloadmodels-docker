/**
 *  random -- random number generators
 */


#ifndef __RANDLIB_H
#define __RANDLIB_H



#include <time.h>

#include "randlib.h"



inline void StartRandom() {
	setall( (long) time(NULL), (long) time(NULL) / 2 );
}

inline double NormalRandom( double mean, double sd ){
	return gennor( mean, sd );
}

inline double GammaRandom( double alpha, double beta ){
	return gengam( 1 / beta, alpha );
}

inline long UniformRandom( long low, long high ){
	return ignuin( low, high );
}

inline int UniformRandom( int low, int high ){
	return (int) UniformRandom( (long) low, (long) high );
}

double UniformRandom( double low, double high );

double UniLogRandom( double logf, double constf );

double JointUniLogRandom( double logxyf, double logxf, double logyf, double constf, double x );

double ProbRandom();

int UniformBit();


#endif