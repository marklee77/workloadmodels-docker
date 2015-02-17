/**
 *  sss - Space Shared Scheduler
 *  Downey model
 */


#include <math.h>

#include "util.h"
#include "clock.h"
#include "downey.h"


/**
 *  Returns the speedup for a downey job of characteristcis #A#, #sigma#, running on #nodes# nodes.
 */

double downeyspeedup( double A, double sigma, int nodes )
{

	double speedup;

	if( nodes == 1 ){
		return 1.0;
	}

	if( sigma <= 1.0 ){
		if( nodes <= A ){
			speedup = ( A * nodes ) / ( A + sigma/2.0 * (nodes - 1.0) );
		}
		else if( nodes <= ( 2.0 * A - 1.0 ) ){
			speedup = ( A * nodes ) / ( sigma * (A - 0.5) + nodes * (1.0 - sigma/2.0) );
		}
		else {
			speedup = A;
		}
	}

	else { // sigma > 1.0
		if( nodes <= A + A*sigma - sigma ){
			speedup = ( nodes * A * (sigma + 1.0) )
				    / ( sigma * (nodes + A - 1.0) + A );
		}
		else {
			speedup = A;
		}
	}

	return speedup;

}


/**
 *  Return the maximum partition size that can benefit a job characterized by #A# and #sigma#.
 */

int downeymaxnodes( double A, double sigma )
{
	if( sigma <= 1.0 ){
		return (int) ceil( 2.0 * A - 1.0 );
	}

	else { // sigma > 1.0
		return (int) ceil( A + A*sigma - sigma );
	}
}


/**
 *  Returns the execution time for a downey job of characteristcis #A#, #sigma#, #L#, running on 
 *  #nodes# nodes.
 */

Time downeyexectime( double A, double sigma, Time L, int nodes )
{
	return max( (Time) 1, (Time) ceil( L / downeyspeedup( A, sigma, nodes ) ) );
}


/**
 *  Returns the serial execution time (L) of a downey job of characteristcis #A#, #sigma#,
 *  given that its execution time with #nodes# nodes is #exectime#.
 */

Time downeyL( double A, double sigma, int nodes, Time exectime )
{
	return max( (Time) 1, (Time) ceil( downeyspeedup( A, sigma, nodes ) * exectime ) );
}
