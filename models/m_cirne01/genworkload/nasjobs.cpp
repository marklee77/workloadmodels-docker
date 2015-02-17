/**
 *  sss - Space Shared Scheduler
 *  nasbench 
 **/




#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "util.h"
#include "nasjobs.h"
#include "random.h"



/****
 *    Create the Job Execution Time
 */


NasJobs::NasJobs( char* exectimes_fn, int availnodes )
{

	FILE* exectimes_fp;
	if( ( exectimes_fp = fopen( exectimes_fn, "r" ) ) == NULL ){
		fatal1( "could not open \"%s\"", exectimes_fn );
	}

	for( NasBench nasbench = nb_first; nasbench <= nb_last; nasbench++ ){
		for( NasClass nasclass = nc_first; nasclass <= nc_last; nasclass++ ){

			// skip comments 
			char line[MAX_LINE + 1];
			do {
				if( fgets( line, MAX_LINE, exectimes_fp ) == NULL ){
					fatal1( "error reading \"%s\"", exectimes_fn );
				}
			} while( ! isdigit( line[0] ) );

			// read information
			int option = 0;
			do {
				sscanf( line, "%i %i", &(nj_nodestable[nasbench][nasclass][option]), &(nj_exectimetable[nasbench][nasclass][option]) );
				option++;
				if( option >= NJ_MAX_OPTIONS ){
					fatal( "more options then allowed by MAX_OPTIONS" );
				}
				if( fgets( line, MAX_LINE, exectimes_fp ) == NULL ){
					fatal1( "error reading \"%s\"", exectimes_fn );
				}
			} while( isdigit( line[0] ) );
			nj_options[nasbench][nasclass] = option;

		}
	}

	fclose( exectimes_fp );

	nj_availnodes = availnodes;

}


Time NasJobs::exectime( NasBench nasbench, NasClass nasclass, int nodes )
{
	// answer for EP jobs
	if( nasbench == nb_EP ){
		return nj_exectimetable[nb_EP][nasclass][0] / nodes;	
	}

	// answer for constrained jobs
	int option; 
	for( option = 0; option < nj_options[nasbench][nasclass]; option++ ){
		if( nj_nodestable[nasbench][nasclass][option] == nodes ){
			break;
		}
	}
	if( option == nj_options[nasbench][nasclass] ){
		fatal3( "the exectime of NAS benchmarks %s, class %s, on %d nodes is unknown", benchname(nasbench), classname(nasclass), nodes );
	}
	return nj_exectimetable[nasbench][nasclass][option];
}




/****
 *    Options on the number of nodes
 */


/**
 *  Returns the first option in terms of number of nodes for a NAS benchmark of type
 *  #nasbench#, class #nasclass#, when running over a supercomputer with #nodes# nodes.
 *  If there are no options, 0 is returned.
 */

int NasJobs::getfirstoption( NasBench nasbench, NasClass nasclass )
{
	nj_bench = nasbench;
	nj_class = nasclass;
	nj_optionindex = 0;
	nj_epoption = 1;
	return getnextoption();
}


/**
 *  Returns the next option in terms of number of nodes, according the parameters set by
 *  getfirstoption(). If there is no option left, 0 is returned.
 */

int NasJobs::getnextoption()
{
	// EP has special treatment
	if( nj_bench == nb_EP ){
		if( nj_epoption <= nj_availnodes ){
			return nj_epoption++;
		}
		else {
			return 0;
		}
	}

	else  // other benchmark then EP
	if( nj_optionindex >= nj_options[nj_bench][nj_class] ||
		nj_nodestable[nj_bench][nj_class][nj_optionindex] > nj_availnodes ){
		return 0;
	}
	else {
		return nj_nodestable[nj_bench][nj_class][nj_optionindex++];
	}
}


/**
 *  Randomly returns one possible option in terms of number of nodes for a NAS benchmark 
 *  of type #nasbench#, class #nasclass#. If there are no options, 0 is returned.
 */

int NasJobs::genrandomoption( NasBench nasbench, NasClass nasclass )
{
	if( nasbench == nb_EP ){
		return UniformRandom( 1, nj_availnodes );
	}
	else {
		int options; 
		for( options = 0; options < nj_options[nasbench][nasclass]; options++ ){
			if( nj_nodestable[nasbench][nasclass][options] > nj_availnodes ){
				break;
			}
		}
		return nj_nodestable[nasbench][nasclass][UniformRandom(0,options-1)];
	}
}


/**
 *  Returns whether #nodes# is a possible option for a NAS benchmark of type #nasbench#,
 *  class #nasclass#.
 */

bool NasJobs::isanoption( NasBench nasbench, NasClass nasclass, int nodes )
{
	if( nodes < 0 ){
		return false;
	}

	if( nasbench == nb_EP ){
		if( nodes > nj_availnodes ){
			return false;
		}
		else {
			return true;
		}
	}
	else {
		for( int option = 0; option < nj_options[nasbench][nasclass]; option++ ){
			if( nj_nodestable[nasbench][nasclass][option] == nodes ){
				return true;
			}
		}
		return false;
	}
}


/**
 *  Returns the number of option available the NAS benchmark #nasbench#, class #nasclass#
 */

int NasJobs::options( NasBench nasbench, NasClass nasclass )
{
	if( nasbench == nb_EP ){
		return nj_availnodes;
	}
	else {
		int options; 
		for( options = 0; options < nj_options[nasbench][nasclass]; options++ ){
			if( nj_nodestable[nasbench][nasclass][options] > nj_availnodes ){
				break;
			}
		}
		return options;
	}
}


/**
 *  Returns the #option#-est available option for the NAS benchmark #nasbench#, class #nasclass#.
 */

int NasJobs::option( NasBench nasbench, NasClass nasclass, int option )
{
	if( nasbench == nb_EP ){
		if( option >= nj_availnodes ){
			fatal( "this option doesn't exist" );
		}
		else {
			return option + 1;
		}
	}

	else {  // isn't EP
		if( option >= nj_options[nasbench][nasclass] ){
			fatal( "this option doesn't exist" );
		}
		else {
			return nj_nodestable[nasbench][nasclass][option];
		}
	}

	return 0; // get rid of a warning
}




/****
 *    Interface-support
 */


/**
 *  Return the human-readable name of #nasbench#
 */

char* benchname( NasBench nasbench )
{
	static char* benchnames[] = { "MG", "LU", "SP", "BT", "EP" };
	return benchnames[nasbench];
}


/**
 *  Return the human-readable name of #nasclass#
 */

char* classname( NasClass nasclass )
{
	static char* classnames[] = { "B", "C" };
	return classnames[nasclass];
}



