/*****
 *
 *  genworkload - generates a synthetic workload
 *  TO DO: Limit the job size.
 */



#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#ifdef AIX
#	include <strings.h>
#endif

#include "util.h"
#include "list.h"
#include "random.h"
#include "jobspec.h"
#include "jobstream.h"


static void usage( char* prog_name );


int main( int argc, char** argv )
{

	// interpert command line
	//

	if( argc != 5 && argc != 6 && argc != 7 ){
		usage( argv[0] );
		exit( -1 );
	}

	int availnodes = atoi( argv[1] );
	if( availnodes <= 0 ){
		usage( argv[0] );
		exit( -1 );
	}

	ArrivalPattern ap;
	if( strcmp( argv[2], "anl" ) == 0 ){
		ap = ap_anl;
	}
	else 
	if( strcmp( argv[2], "ctc" ) == 0 ){
		ap = ap_ctc;
	}
	else 
	if( strcmp( argv[2], "kth" ) == 0 ){
		ap = ap_kth;
	}
	else 
	if( strcmp( argv[2], "sdsc" ) == 0 ){
		ap = ap_sdsc;
	}
	else {
		usage( argv[0] );
		exit( -1 );
	}

	int jobs = atoi( argv[3] );
	if( jobs <= 0 ){
		usage( argv[0] );
		exit( -1 );
	}

	char* workload_name = argv[4];

	double k;
	if( argc == 6 ){
		k = atof( argv[5] );
		if( k <= 0 ){
			usage( argv[0] );
			exit( -1 );
		}
	}
	else {
		k = 1;
	}

	int reductionfactor;
	if( argc == 7 ){
		reductionfactor = atoi( argv[6] );
		if( reductionfactor <= 0 ){
			usage( argv[0] );
			exit( -1 );
		}
	}
	else {
		reductionfactor = 1;
	}
	
	// set-up
	//

	StartRandom();
	JobStream* jobstream = new JobStream( ap, availnodes, k, reductionfactor );
	List<JobSpec>* jobspeclist = new List<JobSpec>;


	// generate synthetic workload
	//

	int j;
	for( j = 0; j < jobs; j++ ){
		
		Time arrival = jobstream->arrival();
		int nodes = jobstream->nodes();
		Time reqtime = jobstream->reqtime();
		double accuracy = jobstream->accuracy();
		Time exectime = max( (Time) 1, (Time) ceil( reqtime * accuracy ) );
		Time start;
		Time finish;
		JobSpecStatus status;
		if( jobstream->cancelled() ){
			start = 0;
			finish = arrival + jobstream->cancellag();
			status = jss_cancelled;
		}
		else {
			start = arrival;
			finish = arrival + exectime;
			status = jss_completed;
		}
		
		JobSpec* jobspec = new JobSpec( NULL, "0", "0", "synthetic", arrival, nodes, reqtime, start, finish, status );

		jobspeclist->add( jobspec );

	}


	// write results and quit
	//

	writestatic( jobspeclist, workload_name, "workload" );

    return 0;

}


static void usage( char* prog_name )
{
	cerr << "usage: " << prog_name << " <nodes> <arrivalpattern> <jobs> <workload> [<k> <reductionfactor>]" << endl
		 << "       where: <nodes> is how many nodes the supercomputer has" << endl
		 << "              <arrivalpattern> == anl | ctc | kth | sdsc" << endl
		 << "              <jobs> is the number of jobs to generate" << endl
		 << "              <workload>.workload contains the output" << endl
		 << "              <k> is the load multiplier (default is 1)" << endl
		 << "              <reductionfactor> will minimize the job size but keep the load constant (default is 1)" << endl;
}


