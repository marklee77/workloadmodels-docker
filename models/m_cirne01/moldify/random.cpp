/**
 *  random -- random number generators
 */



#include <math.h>

#include "com.h"
#include "util.h"
#include "randlib.h"
#include "random.h"



/**
 *  A random number from the uniform log distribution of parameters #logf# and #constf#.
 */

double UniLogRandom( double logf, double constf )
{
	double cdf = ProbRandom();
	double n = pow( 2, ( cdf - constf ) / logf );
	return n;
}


/**
 *  A random number from the joint uniform log distribution of parameters #logxff#, #logxf#,
 *  #logyf and #constf#, assuming x == #x#.
 */

double JointUniLogRandom( double logxyf, double logxf, double logyf, double constf, double x )
{
	double s_logf = logxyf * logbase( x, 2 ) + logyf;
	double s_constf = logxf * logbase( x, 2 ) + constf;
	return UniLogRandom( s_logf, s_constf );
}


/**
 *  Uniformly generates a double in the interval [0, 1]
 */

double ProbRandom()
{
	const long maxnum = 2147483562 - 1;
	long randnum = ignlgi() - 1;
	return (double) randnum / (double) maxnum;
}


/**
 *  Uniformly generates a double in the interval [low, high]
 */

double UniformRandom( double low, double high )
{
	if( low > high ){
		fatal( "low > high in UniformRandom()" );
	}
	return low + ProbRandom() * ( high - low );
}


/**
 *  Uniformly generates 0 or 1
 */

int UniformBit()
{
	double p = ProbRandom();
	if( p <= 0.5 ){
		return 0;
	}
	else {
		return 1;
	}
}
