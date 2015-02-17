/**
 *  sa - Supercomputer AppLeS
 *  cookworkload
 **/



#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include "strstream.h"
#include <string.h>
#ifdef AIX
#	include <strings.h>
#endif

#include "util.h"
#include "list.h"
#include "downey.h"
#include "jobspec.h"
#include "apples.h"
#include "random.h"
#include "moldability.h"


static void usage( char* prog_name );
static List<JobSpec>* randomizejobs( List<JobSpec>* origjobspeclist );
static void splitjobs( List<JobSpec>* jobspeclist, List<JobSpec>** reservedlist, List<JobSpec>** targetlist, int skipinterval );
static void jobscat( List<JobSpec>* target, List<JobSpec>* source );


void main( int argc, char** argv )
{


	// variables
	//

	typedef enum { re_nas, re_ep, re_downey } Replacement;
	typedef enum { pm_as_original, pm_value, pm_random } PerfModel;

	char* workload_fn = NULL;
	FileName workload_name;
	int availnodes = 0;
	PerfModel perfmodel;
	double accuracy;
	Replacement replacement;
	PartSizeDist partsizedist;
	char* exectime_fn = NULL;
	int replacepercent = 0;
	int jobstoreplace = 0;
	double k_sigma = 0.0;
	
	// interpert command line
	//

	if( argc < 5 || argc > 7 ){
		usage( argv[0] );
		exit( -1 );
	}
	workload_fn = argv[1];
	basename( workload_name, workload_fn );
	availnodes = atoi( argv[2] );		
	if( strcasecmp( argv[3], "as-original" ) == 0 ){
		perfmodel = pm_as_original;
		accuracy = 0.0;
	} else
	if( strcasecmp( argv[3], "random" ) == 0 ){
		perfmodel = pm_random;
		accuracy = 0.0;
	} else 
	if( strcasecmp( argv[3], "perfect" ) == 0 ){
		perfmodel = pm_value;
		accuracy = 1.0;
	}
	else {
		perfmodel = pm_value;
		accuracy = atof( argv[3] );
		if( accuracy <= 0 || accuracy > 1 ){
			usage( argv[0] );
			exit( -1 );
		}
	}
	if( strcasecmp( argv[4], "nas" ) == 0 ){
		replacement = re_nas;
		exectime_fn = argv[5];
	} else
	if( strcasecmp( argv[4], "ep" ) == 0 ){
		replacement = re_ep;
		exectime_fn = argv[5];
	} else 
	if( strcasecmp( argv[4], "downey" ) == 0 ){
		replacement = re_downey;
		if( strcasecmp( argv[5], "com" ) == 0 ){
			partsizedist = psd_continous;
		} else
		if( strcasecmp( argv[5], "p2bias" ) == 0 ){
			partsizedist = psd_p2_bias;
		}
		else {
			usage( argv[0] );
			exit( -1 );
		}
		if( argc == 7 ){
			k_sigma = atof( argv[6] );
			if( k_sigma <= 0 ){
				usage( argv[0] );
				exit( -1 );
			}
		}
		else {
			k_sigma = 1.0;
		}
	}
	else {
		usage( argv[0] );
		exit( -1 );
	}


	// set-up
	//

	StartRandom();

	List<JobSpec>* jobspeclist = new List<JobSpec>;
	readjobs( workload_fn, NULL, jobspeclist, false );
	NasJobs* nasjobs;


	// nas jobs
	//

	if( replacement == re_nas || replacement == re_ep ){

		jobspeclist = randomizejobs( jobspeclist );

		nasjobs = new NasJobs( exectime_fn, availnodes );
		NasBench nasbench = nb_first;
		NasClass nasclass = nc_first;
		int nodes = 0;
		JobSpec* jobspec = NULL;
		
		while( true ){
			if( jobspec == NULL ){
				if( replacement == re_nas ){
					nasbench = UniformRandom( 0, nb_last );
				}
				if( replacement == re_ep ){
					nasbench = nb_EP;
				}
				nasclass = UniformRandom( 0, nc_last );
				jobspec = jobspeclist->getfirst();
			}
			if( nasjobs->isanoption( nasbench, nasclass, jobspec->js_nodes ) ){
				break;
			}
			jobspec = jobspeclist->getnext();
		}

		if( perfmodel == pm_random ){
			jobspec->js_accuracy = UniformRandom( 0.0, 1.0 );
		}
		else
		if( perfmodel == pm_as_original ){
			Time exectime; 
			if( jobspec->js_status == jss_completed ){
				exectime = jobspec->js_finish - jobspec->js_start;
			}
			else { 
				exectime = UniformRandom( 1, jobspec->js_reqtime );
			}
			jobspec->js_accuracy = ( (double) exectime ) / ( (double) jobspec->js_reqtime );
			jobspec->js_accuracy = min( 1.0, jobspec->js_accuracy );
		}
		else
		if( perfmodel == pm_value ){
			jobspec->js_accuracy = accuracy;
		}
		jobspec->js_reqtime = (Time) ( nasjobs->exectime( nasbench, nasclass, jobspec->js_nodes ) / jobspec->js_accuracy );
		jobspec->js_finish = jobspec->js_start + nasjobs->exectime( nasbench, nasclass, jobspec->js_nodes );
		jobspec->js_bench = nasbench;
		jobspec->js_class = nasclass;

		strcpy( jobspec->js_tag, str_modified );
		jobspec->js_status = jss_completed;
		jobspec->js_type = jst_nas;

		// write results
		writestatic( jobspeclist, workload_name );
		writebushel( jobspeclist, workload_name );
		writenasoptions( jobspeclist, nasjobs->options( nasbench, nasclass ), nasjobs, availnodes, workload_name );

	}


	// downey (evaluation of the Bushel of AppLeS)
	// - the status of moldified jobs is not altered
	//

	if( replacement == re_downey ){

		jobspeclist = randomizejobs( jobspeclist );
		jobstoreplace = jobspeclist->numelems();

		int targetoptions = 0;
		bool targetjob = true;
		JobSpec* jobspec = jobspeclist->getfirst();

		while( jobstoreplace > 0 ){

			int cmin = gencmin();
			jobspec->js_sigma = k_sigma * gensigma();
			jobspec->js_A = genA( cmin );
			int cmax = downeymaxnodes( jobspec->js_A, jobspec->js_sigma );
			cmin = min( cmin, cmax );
			
			Time exectime; 
			if( jobspec->js_status == jss_completed ){
				exectime = jobspec->js_finish - jobspec->js_start;
			}
			else { 
				exectime = UniformRandom( 1, jobspec->js_reqtime );
			}
			jobspec->js_L = downeyL( jobspec->js_A, jobspec->js_sigma, jobspec->js_nodes, exectime );

			if( perfmodel == pm_random ){
				jobspec->js_accuracy = UniformRandom( 0.0, 1.0 );
				jobspec->js_accuracy = min( 1.0, jobspec->js_accuracy );
			}
			else
			if( perfmodel == pm_as_original ){
				jobspec->js_accuracy = ((double) exectime) / ((double) jobspec->js_reqtime);
				jobspec->js_accuracy = min( 1.0, jobspec->js_accuracy );
			}
			else
			if( perfmodel == pm_value ){
				jobspec->js_accuracy = accuracy;
			}

			// to get rid of a small rounding error, we calculate exectime again:
			exectime = downeyexectime( jobspec->js_A, jobspec->js_sigma, jobspec->js_L, jobspec->js_nodes );
			jobspec->js_reqtime = max( (Time) 1, (Time) ( exectime / jobspec->js_accuracy ) );
			if( targetjob ){
				// status is always completed
				jobspec->js_status = jss_completed;
				jobspec->js_start = jobspec->js_queued;
				jobspec->js_finish = jobspec->js_start + exectime;
				jobspec->js_canceled = 0;
			}
			else {
				// status is not altered
				if( jobspec->js_status == jss_completed ){
					jobspec->js_finish = jobspec->js_start + exectime;
					jobspec->js_canceled = 0;
				}
				else
				if( jobspec->js_status == jss_cancelled ){
					jobspec->js_canceled = jobspec->js_finish;
				}
				else {
					fatal1( "unknown jobspec status: %d", jobspec->js_status );
				}
			}

			// generate possible cu
			CAlg calg = genconstraint( partsizedist, jobspec->js_nodes ); 
			int cu = gencu( calg );
			int options = jobspec->js_options = genoptions( cmin, calg, cu, jobspec->js_A, jobspec->js_sigma, availnodes, jobspec->js_option );	
			jobspec->js_option[0] = jobspec->js_nodes;

			// set tags and the such
			if( targetjob ){
				strcpy( jobspec->js_tag, str_modified );
				targetoptions = options;
				targetjob = false;
			}
			ostrstream s;
			s << jobspec->js_tag << "-" << cmin << "-" << cmax << "-" << constraintname( calg ) << ends; 
			strcpy( jobspec->js_tag, s.str() );
			jobspec->js_type = jst_downey ;
			
			// book keeping
			jobstoreplace--;
			jobspec = jobspeclist->getnext();

		}

		// write results
		writestatic( jobspeclist, workload_name );
		writetarget( jobspeclist, workload_name );
		writedowneyoptions( jobspeclist, targetoptions, workload_name );
		writenaive( jobspeclist, workload_name );
		writebushel( jobspeclist, workload_name );

	}


	exit( 0 );


}


static void usage( char* prog_name )
{
	cerr << "usage: " << prog_name << " <workload> <nodes> <accuracy> <replacement> <nas-exec-times>" << endl
	     << "   or: " << prog_name << " <workload> <nodes> <accuracy> <replacement> <partsizedist> [<k_sigma>]" << endl
		 << "       where: <workload> contains the original workload" << endl
		 << "              <nodes> is number of nodes of the target supercomputer" << endl
		 << "              <accuracy> == perfect | as-original | random | <value>" << endl
		 << "              <replacement> == nas | ep | downey" << endl
		 << "              if <replacement> == nas | ep, you need to specify:" << endl
		 << "                     <nas-exec-times> contains the execution times of the NAS benchmark" << endl
		 << "              if <replacement> == downey, you need to specify:" << endl
		 << "                     <partsizedist> == cont | p2bias" << endl
		 << "                     <k_sigma> == the multiplier for sigma (default == 1)" << endl
		 << "       output: two files named <workload>.static and <workload>.apples" << endl;		
}


static List<JobSpec>* randomizejobs( List<JobSpec>* origjobspeclist )
{
	List<JobSpec>* result = new List<JobSpec>;
	JobSpec* jobspec;
	int serialnumber = 1;
	while( (jobspec =  origjobspeclist->removerandomly()) != NULL ){
		jobspec->setserialnumber( serialnumber );
		result->add( jobspec );
		serialnumber++;
	}
	return result;
}


static void splitjobs( List<JobSpec>* jobspeclist, List<JobSpec>** reservedlist, List<JobSpec>** targetlist, int skipinterval )
{
	*reservedlist = new List<JobSpec>;
	*targetlist = new List<JobSpec>;
	int i = 0;
	JobSpec* jobspec;
	for( i = 0, jobspec = jobspeclist->getfirst(); 
		 jobspec != NULL;
		 i++, jobspec = jobspeclist->getnext() ){
		if( i % (skipinterval + 1 ) == 0 ){
			(*targetlist)->add( jobspec );
		}
		else {
			(*reservedlist)->add( jobspec );
		}
	}
}


static void jobscat( List<JobSpec>* target, List<JobSpec>* source )
{
	for( JobSpec* jobspec = source->getfirst(); jobspec != NULL; jobspec = source->getnext() ){
		target->add( jobspec );
	}
}


