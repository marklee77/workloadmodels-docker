/**
 *  moldability model
 */



#include "util.h"
#include "downey.h"
#include "random.h"
#include "moldability.h"


const double cmin_ul1 = 0.0692;
const double cmin_ul2 = 0.6279;

const double calg_p[] = { 0.02, 0.10, 0.02, 0.08, 0.08, 0.70 };

const double cu_1 = 0.0306;
const double cu_gt1_ul1 = 0.1918; 
const double cu_gt1_ul2 = 0.1876;

const double A_julxy = 0.009548;
const double A_julx = -0.01877;
const double A_july = 0.07468;
const double A_julconst = -0.009198;

const double sigma_mean = 1.209;
const double sigma_sd = 1.132;



/**
 *
 */

char* constraintname( CAlg calg )
{
	static char* names[] = { "rigid", "powerof2", "square", "even", "multipleof4", "unconstraint" };
	return names[calg];
}


/**
 *
 */

CAlg genconstraint( PartSizeDist partdistsize, int nodes )
{

	if( partdistsize == psd_continous ){
		return c_unconstraint;
	}

	else
	if( partdistsize == psd_p2_bias ){		// that's the current model
		if( ispowerof2( nodes ) ){
			return c_powerof2;
		}
		else {
			return c_unconstraint;
		}
	}

	else
	if( partdistsize == psd_job_nature ){
		double cdf = ProbRandom();
		double accul = 0;
		for( int i = 0; i <= c_last; i++ ){
			if( cdf <= accul + calg_p[i] ){
				return (CAlg) i;
			}
			accul += calg_p[i];
		}
		fatal( "genconstraint() should never get to this point" );
	}

	return c_first; // this gets rid of a warning
}


/**
 *
 */

int gencmin()
{
	return max( 1, (int) ceil( UniLogRandom( cmin_ul1, cmin_ul2 ) ) );
}


/**
 *  Note that c_u = v + 1, where v is the number of alternative requests
 */

int gencu( CAlg calg )
{
	int cu;
	if( calg == c_rigid ){		// cu_1 is not considered because c_min / c_max are already creating a large number of rigid jobs
		cu = 1;
	}
	else {
		cu = max( 2, (int) ceil( UniLogRandom( cu_gt1_ul1, cu_gt1_ul2 ) ) );
	}
	return cu;
}


/**
 *
 */

double gensigma()
{
	return max( 0.0, NormalRandom( sigma_mean, sigma_sd ) );
}


/**
 *
 */

double genA( int cmin )
{
	return max( 1.0, JointUniLogRandom( A_julxy, A_julx, A_july, A_julconst, cmin ) );
}


/**
 *  It returns how many options were generated. The options themselves are stored in #result#.
 */

int genoptions( int cmin, CAlg calg, int cu, double A, double sigma, int availnodes, int* result )
{

	// generate possible options
	int *posoption = new int[availnodes];	
	int posoptions = 0;
	int option;
	if( calg == c_powerof2 || calg == c_square || calg == c_unconstraint || calg == c_rigid ){
		option = 1;
	}
	else if( calg == c_even ){
		option = 2;
	}
	else if( calg == c_multof4 ){
		option = 4;
	}
	else {
		fatal( "unknown calg" );
	}
	int cmax = min( downeymaxnodes( A, sigma ), availnodes );
	while( posoptions == 0 || option <= cmax ){
		if( option >= cmin ){
			posoption[posoptions++] = option;
		}
		if( calg == c_powerof2 ){
			option *= 2;
		}
		else if( calg == c_square ){
			option = ( (int) sqrt( option ) + 1 ) * ( (int) sqrt( option ) + 1 );
		}
		else if( calg == c_even ){
			option += 2;
		}
		else if( calg == c_multof4 ){
			option += 4;
		}
		else if( calg == c_unconstraint ){		
			option++;
		}
		else if( calg == c_rigid ){		
			option++;		// to make that we get over cmin
		}
		else {
			fatal( "unknown calg" );
		}
	}

	// randomly get #cu# options out of all possible options
	shuffle( posoption, posoptions );
	int options = min( posoptions, cu );
	for( option = 0; option < options; option++ ){
		result[option] = posoption[option];
	}

	// clean up and leave
	delete posoption;
	return options;

}


