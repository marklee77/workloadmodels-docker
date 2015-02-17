/*****
 *
 *  parameters for a stream of jobs
 *  
 */



#include "util.h"
#include "random.h"
#include "jobstream.h"


// the coeficient of ms are those of the thesis divided by 60 (to convert the rate from minutes to seconds)
const int ms_degree = 13;
double ms[][ms_degree + 1] = {
	{  0.000000E+00,  1.194417E+02, 1.069567E+03,  1.443500E+01, -7.552833E+02, -4.871500E+01,  1.960167E+02,  1.491917E+01, -2.248333E+01, -1.554433E+00,  1.041417E+00,  3.766833E-02, -1.102533E-02, 2.865167E-03 },
	{  0.000000E+00,  0.000000E+00, 0.000000E+00,  0.000000E+00,  0.000000E+00,  4.234000E+00, -4.303333E-01, -4.308500E+00,  1.407367E-01,  1.360200E+00, -6.104667E-02, -1.605150E-01,  1.274250E-02, 9.416833E-03 },
	{ -5.447167E+02, -1.024067E+03, 5.425500E+02,  8.234333E+02, -2.101833E+02, -2.518833E+02,  3.968167E+01,  3.590000E+01, -3.694167E+00, -2.247000E+00,  1.383983E-01,  3.195833E-02,  3.734333E-04, 1.618433E-03 },
	{  0.000000E+00,  0.000000E+00, 0.000000E+00, -9.166000E+01,  2.545000E+01,  5.026500E+01, -1.586583E+01, -9.551333E+00,  3.322000E+00,  8.097167E-01, -2.601833E-01, -4.313167E-02,  6.112500E-03, 3.543667E-03 },
};

const double n_ul1 = 0.12;
const double n_ul2 = 0.20;
const double n_p2_frac = 0.75;

const double tr_ul1 = 0.10;
const double tr_ul2 = -0.75; 

const double a_g1 = 0.6;
const double a_g2 = 0.6;

const double pc = 0.15;
const double cl_ul1 = 0.065;
const double cl_ul2 = -0.32;



/**
 *  Returns the next arrival.
 */

Time JobStream::arrival()
{

	const double minnorm = -0.5;
	const double maxnorm = +0.5;
	const double normrange = maxnorm - minnorm;
	const double secsperday = 24 * 60 * 60;
	const double step = normrange / secsperday;

	if( js_initarrival ){
		js_arrivalday = 0;
		js_arrivalnorm = UniformRandom( minnorm, maxnorm );
		js_arrivalacculrate = 0;
		js_initarrival = false;
	}

	// 2.0 is used because arrival.m smoothes the arrival rate by averaging it over 2 minutes
	while( js_arrivalacculrate < 2.0 ){
		js_arrivalnorm += step;
		if( js_arrivalnorm > maxnorm ){
			js_arrivalday++;
			js_arrivalnorm -= normrange;
		}
		js_arrivalacculrate += js_k * js_reductionfactor * js_scalefactor * polyeval( ms_degree, ms[js_ap], js_arrivalnorm );
	}
	js_arrivalacculrate -= 2.0;

	return (Time) ceil( secsperday * ( js_arrivalday + ( js_arrivalnorm - minnorm ) / normrange ) );
	
}


/**
 *  Returns how many nodes this submission will ask.
 */

int JobStream::nodes()
{
	int n = (int) ceil( UniLogRandom( n_ul1, n_ul2 ) );

	n = max( 1, n );
	if( ProbRandom() <= n_p2_frac ){
		n = (int) pow( 2, round( logbase( n, 2.0 ) ) );
	}
	n = min( js_maxnodes, n );

	if( js_reductionfactor == 2 && n > 1 ){
		if( n % 2 == 0 ){
			n = n / 2;
		}
		else {
			n = ( n / 2 ) + UniformBit();
		}
	}

	return n;
}


/**
 *
 */

Time JobStream::reqtime()
{
	return max( (Time) 1, (Time) ceil( js_k * UniLogRandom( tr_ul1, tr_ul2 ) ) );
}


/**
 *
 */

double JobStream::accuracy()
{
	double a = GammaRandom( a_g1, a_g2 );
	a = max( a, 1E-5 );
	a = min( a, 1.0 );
	return a;
}


/**
 *
 */

bool JobStream::cancelled()
{
	return ( ProbRandom() <= pc );
}


/**
 *
 */

Time JobStream::cancellag()
{
	return max( (Time) 1, (Time) ceil( UniLogRandom( cl_ul1, cl_ul2 ) ) );
}
