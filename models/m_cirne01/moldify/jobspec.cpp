/**
 *  sss - Space Shared Scheduler
 *  JobSpec - Simulator subsystem
 */


#include <math.h>
#include <fstream.h>
#include <iostream.h>


#define __JOBSPEC_CPP
#include "jobspec.h"
#include "nasjobs.h"
#include "downey.h"
#include "apples.h"
#include "event.h"
#include "simulator.h"



/**
 *  Initialization
 */

int JobSpec::js_nextserialnumber = 1;



/**
 *  Submits a job to #sss# and does all the necessary bookkeeping. The bookkeeping consists of
 *  (i) adding a cancellation to the event list (when necessary), and (ii) updating the home
 *  supercomputer (also when necessary).
 */

void JobSpec::submit( SSS* sss, int requests, Request* request, char* tag, CallBackFunc starthandler, CallBackFunc interrupthandler, void* handlerparam )
{
#ifdef SIMULATOR

	if( js_submitted ){
		if( js_ssss >= MAX_SUPERCOMPUTERS ){
			fatal( "more supercomputers than MAX_SUPERCOMPUTERS" );
		}
		js_sss[js_ssss] = sss;
		js_ssss++;
	}
	else {
		js_sss[0] = sss;
	}
	js_submitted = true;

	Job* job = sss->submit( requests, request, tag, starthandler, interrupthandler, handlerparam );

	if( js_type == jst_fixed && js_status == jss_cancelled ){
		Event* removal = new Event( js_finish, et_cancel, job, sss );
		g_elist->add( removal );
	}
	if( js_type == jst_downey && js_canceled != 0 ){
		Event* removal = new Event( js_canceled, et_cancel, job, sss );
		g_elist->add( removal );
	}

#endif
}


/**
 *  Returns the exectime of the job when running on #nodes# nodes on supercomputer #sss#
 */

Time JobSpec::exectime( int nodes, SSS* sss )
{

	if( js_type == jst_fixed && nodes != js_nodes ){
		fatal1( "getting exectime for a different number of nodes of fixed job %s", js_name );
	}

	if( js_type == jst_fixed ){
		if( js_status == jss_completed ){
			return js_finish - js_start;
		}
		else { 
			// this has to be deterministic to keep fair the comparison among multiple schedulers 
			return max( (Time) 1, (Time) ( js_reqtime / 2 ) );
		}
	} 

	else 
	if( js_type == jst_downey ){ 
		return downeyexectime( js_A, js_sigma, js_L, nodes );	
	} 

	else 
	if( js_type == jst_nas ){ 
		return sss->sss_nasjobs->exectime( js_bench, js_class, nodes );
	}

	else {
		fatal1( "unknown js_type: %s", js_type );
	}
	return 0;	// this avoids a warning

}



/**
 *  Return how many requests a job carries.
 */

int JobSpec::requests()
{
	if( js_type == jst_fixed ){
		return 1;
	} else

	if( js_type == jst_downey ){ 
		return js_options;
	} else

	if( js_type == jst_nas ){ 
		fatal( "JobSpec::requests() hasn't been implemented yet" );
	}

	else {
		fatal1( "unknown js_type: %s", js_type );
	}
	return 0;	// this avoids a warning
}



/**
 *  Return a vector with all requests a job carries. Times are calculated for supercomputer #sss#.
 */

Request* JobSpec::request( SSS* sss )
{
	if( js_type == jst_fixed ){
		Request* request = new Request;
		request->r_nodes = js_nodes;
		request->r_reqtime = js_reqtime;
		return request;
	} else

	if( js_type == jst_downey ){ 
		Request* request = new Request[js_options];
		int r;
		for( r = 0; r < js_options; r++ ){
			request[r].r_nodes = js_option[r];
			request[r].r_reqtime = (Time) ( exectime( js_option[r], sss ) / js_accuracy );
		}
		return request;
	} else

	if( js_type == jst_nas ){ 
		fatal( "JobSpec::request() hasn't been implemented yet" );
	}

	else {
		fatal1( "unknown js_type: %s", js_type );
	}
	return 0;	// this avoids a warning
}



/**
 *  Read the jobs' descriptions stored in #jobsfile#, and store such descriptions in
 *  #jobspeclist#. #sc# is the supercomputer to which the jobs should be submitted.
 *  #richrequests# tells whether downey and/or nas requests can be present in #jobsfile#. 
 */

void readjobs( char* jobsfile, SSS* sc, List<JobSpec>* jobspeclist, bool richrequests )
{

	if( sc != NULL ){
		jobspeclist = sc->sss_jobspeclist;
	}

	ifstream jobs( jobsfile, ( ios::nocreate | ios::in ) );
	if( jobs == NULL ){
		fatal( "could not open the jobs file" );
	}

	char name[MAX_NAME + 1];
	char owner[MAX_NAME + 1];
	char tag[MAX_NAME + 1];   
	char str_type[MAX_NAME + 1];
	Time queued;
	JobSpec* jobspec;

	while( true ){

		jobs >> name >> owner >> tag >> str_type >> queued;
		if( jobs.eof() ){
			break;
		}
		if( jobs.fail() || jobs.bad() ){
			fatal1( "problems reading job named \"%s\"", name );
		}
		JobSpecType type = genjobspectype( str_type );

		if( type == jst_fixed ){
			int nodes;
			Time maxwalltime;
			Time start;
			Time finish;
			char str_status[MAX_NAME + 1];
			jobs >> nodes >> maxwalltime >> start >> finish >> str_status;
			JobSpecStatus status = genjobspecstatus( str_status );
			if( jobs.eof() || jobs.fail() || jobs.bad() ){
				fatal1( "problems reading job named \"%s\"", name );
			}
			jobspec = new JobSpec( sc, name, owner, tag, queued, nodes, maxwalltime, start, finish, status );
		}

		else if( type == jst_downey ){
			if( ! richrequests ){
				fatal( "rich requests are not expected" );
			}

			Time canceled;
			Time L;
			double A;
			double sigma;
			double accuracy;
			int options;
			jobs >> canceled >> L >> A >> sigma >> accuracy >> options;
			if( jobs.eof() || jobs.fail() || jobs.bad() ){
				fatal1( "problems reading job named \"%s\"", name );
			}
			jobspec = new JobSpec( sc, name, owner, tag, queued, canceled, L, A, sigma, accuracy, options );
			for( int i = 0; i < options; i++ ){
				int option;
				jobs >> option;
				if( jobs.eof() || jobs.fail() || jobs.bad() ){
					fatal1( "problems reading job named \"%s\"", name );
				}
				jobspec->js_option[i] = option;
			}
		}

		else if( type == jst_nas ){
			if( ! richrequests ){
				fatal( "rich requests are not expected" );
			}

			char nasbench[MAX_NAME + 1];
			char nasclass[MAX_NAME + 1];
			double accuracy;
			jobs >> nasbench >> nasclass >> accuracy;
			if( jobs.eof() || jobs.fail() || jobs.bad() ){
				fatal1( "problems reading job named \"%s\"", name );
			}
			jobspec = new JobSpec( sc, name, owner, tag, queued, nasbench, nasclass, accuracy );
		}

		else {
			fatal1( "wrong type for job named \"%s\"", name );
		}

		jobspeclist->add( jobspec );

	}

}



/**
 *  Write #jobspeclist# into the file #workload_fn#.#extension#, making all jobs look static submissions
 */

void writestatic( List<JobSpec>* jobspeclist, char* workload_fn, char* extension )
{

	char staticworkload_fn[ MAX_FILENAME + 1];
	strcpy( staticworkload_fn, workload_fn );
	strcat( staticworkload_fn, "." );
	strcat( staticworkload_fn, extension );

	ofstream staticworkload( staticworkload_fn );
	if( staticworkload == NULL ){
		fatal1( "could not open \"%s\"", staticworkload_fn );
	}

	const char tab = '\t';
	for( JobSpec* jobspec = jobspeclist->getfirst(); jobspec != NULL; jobspec = jobspeclist->getnext() ){
		staticworkload << jobspec->js_name << tab
					   << jobspec->js_owner << tab
					   << jobspec->js_tag << tab
					   << jobspectypename( jst_fixed ) << tab
					   << jobspec->js_queued << tab
					   << jobspec->js_nodes << tab
					   << jobspec->js_reqtime << tab
					   << jobspec->js_start << tab
					   << jobspec->js_finish << tab
					   << jobspecstatusname( jobspec->js_status ) << endl;
		if( staticworkload.eof() || staticworkload.fail() || staticworkload.bad() ){
			fatal1( "could not write to \"%s\"", staticworkload_fn );
		}
	}

}



/**
 *  Write #jobspeclist# into the file #workload_fn#.#extension#. The modified job is written 
 *  as a moldable job. All the rest is written as static.
 */

void writetarget( List<JobSpec>* jobspeclist, char* workload_fn, char* extension )
{

	char applesworkload_fn[ MAX_FILENAME + 1];
	strcpy( applesworkload_fn, workload_fn );
	strcat( applesworkload_fn, "." );
	strcat( applesworkload_fn, extension );

	ofstream applesworkload( applesworkload_fn );
	if( applesworkload == NULL ){
		fatal1( "could not open \"%s\"", applesworkload_fn );
	}

	const char tab = '\t';
	for( JobSpec* jobspec = jobspeclist->getfirst(); jobspec != NULL; jobspec = jobspeclist->getnext() ){

		applesworkload << jobspec->js_name << tab
					   << jobspec->js_owner << tab
					   << jobspec->js_tag << tab;
		if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
			fatal1( "could not write to \"%s\"", applesworkload_fn );
		}
		
		if( jobspec->js_type == jst_fixed || strstr( jobspec->js_tag, str_modified ) == NULL ){
			applesworkload << jobspectypename( jst_fixed ) << tab
						   << jobspec->js_queued << tab
					       << jobspec->js_nodes << tab
						   << jobspec->js_reqtime << tab
						   << jobspec->js_start << tab
						   << jobspec->js_finish << tab
						   << jobspecstatusname( jobspec->js_status ) << endl;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
				fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
		}

		else
		if( jobspec->js_type == jst_downey ){
			applesworkload << jobspectypename( jst_downey ) << tab
					       << jobspec->js_queued << tab
						   << jobspec->js_canceled << tab
						   << jobspec->js_L << tab
						   << jobspec->js_A << tab
						   << jobspec->js_sigma << tab
						   << jobspec->js_accuracy << tab
						   << jobspec->js_options << tab;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
					fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
			for( int i = 0; i < jobspec->js_options; i++ ){
				applesworkload << jobspec->js_option[i] << tab;
				if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
					fatal1( "could not write to \"%s\"", applesworkload_fn );
				}
			}
			applesworkload << endl;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
				fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
		}

		else
		if( jobspec->js_type == jst_nas ){
			applesworkload << benchname( jobspec->js_bench ) << tab
						   << classname( jobspec->js_class ) << tab
						   << jobspec->js_accuracy << endl;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
				fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
		}

	}

}



/**
 *  For all possible #options# of downey jobs in #jobspeclist#, write the jobs into the 
 *  file #workload_fn#.#extension#$option$, making all jobs look static submissions
 */

void writedowneyoptions( List<JobSpec>* jobspeclist, int options, char* workload_fn, char* extension )
{

	char optionworkload_basefn[ MAX_FILENAME + 1];
	strcpy( optionworkload_basefn, workload_fn );
	strcat( optionworkload_basefn, "." );
	strcat( optionworkload_basefn, extension );

	for( int option = 0; option < options; option++ ){

		char optionworkload_fn[ MAX_FILENAME + 1];
		sprintf( optionworkload_fn, "%s%i", optionworkload_basefn, option );

		ofstream optionworkload( optionworkload_fn );
		if( optionworkload == NULL ){
			fatal1( "could not open \"%s\"", optionworkload_fn );
		}

		const char tab = '\t';
		for( JobSpec* jobspec = jobspeclist->getfirst(); jobspec != NULL; jobspec = jobspeclist->getnext() ){

			optionworkload << jobspec->js_name << tab
						   << jobspec->js_owner << tab
						   << jobspec->js_tag << tab
						   << jobspectypename( jst_fixed ) << tab
						   << jobspec->js_queued << tab;
			if( optionworkload.eof() || optionworkload.fail() || optionworkload.bad() ){
				fatal1( "could not write to \"%s\"", optionworkload_fn );
			}
		
			if( jobspec->js_type == jst_fixed || strstr( jobspec->js_tag, str_modified ) == NULL ){
				optionworkload << jobspec->js_nodes << tab
							   << jobspec->js_reqtime << tab
							   << jobspec->js_start << tab
							   << jobspec->js_finish << tab
							   << jobspecstatusname( jobspec->js_status ) << endl;
				if( optionworkload.eof() || optionworkload.fail() || optionworkload.bad() ){
					fatal1( "could not write to \"%s\"", optionworkload_fn );
				}
			}

			else
			if( jobspec->js_type == jst_downey ){
				int nodes = jobspec->js_option[option];
				Time exectime = downeyexectime( jobspec->js_A, jobspec->js_sigma, jobspec->js_L, nodes );
				Time reqtime = max( (Time) 1, (Time) ( exectime / jobspec->js_accuracy ) );
				Time finish;
 				if( jobspec->js_status == jss_completed ){
					finish = jobspec->js_start + exectime;
				} 
				else if( jobspec->js_status == jss_cancelled ){
					finish = jobspec->js_finish;
				}
				else {
					fatal1( "unexpected jobspec status: %d", jobspec->js_status );
				}
				optionworkload << nodes << tab
							   << reqtime << tab
							   << jobspec->js_start << tab
							   << finish << tab
							   << jobspecstatusname( jobspec->js_status ) << endl;
			}

			else
			if( jobspec->js_type == jst_nas ){
				fatal( "a nas job present writedowneyoptions()" );
			}

		}

	}

}



/**
 *  Write #jobspeclist# into the file #workload_fn#.#extension#. The modified job is written 
 *  as a staticjob. All the rest are written as moldable.
 */

void writenaive( List<JobSpec>* jobspeclist, char* workload_fn, char* extension )
{

	char applesworkload_fn[ MAX_FILENAME + 1];
	strcpy( applesworkload_fn, workload_fn );
	strcat( applesworkload_fn, "." );
	strcat( applesworkload_fn, extension );

	ofstream applesworkload( applesworkload_fn );
	if( applesworkload == NULL ){
		fatal1( "could not open \"%s\"", applesworkload_fn );
	}

	const char tab = '\t';
	for( JobSpec* jobspec = jobspeclist->getfirst(); jobspec != NULL; jobspec = jobspeclist->getnext() ){

		applesworkload << jobspec->js_name << tab
					   << jobspec->js_owner << tab
					   << jobspec->js_tag << tab;
		if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
			fatal1( "could not write to \"%s\"", applesworkload_fn );
		}
		
		if( jobspec->js_type == jst_fixed || strstr( jobspec->js_tag, str_modified ) != NULL ){
			applesworkload << jobspectypename( jst_fixed ) << tab
						   << jobspec->js_queued << tab
					       << jobspec->js_nodes << tab
						   << jobspec->js_reqtime << tab
						   << jobspec->js_start << tab
						   << jobspec->js_finish << tab
						   << jobspecstatusname( jobspec->js_status ) << endl;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
				fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
		}

		else
		if( jobspec->js_type == jst_downey ){
			applesworkload << jobspectypename( jst_downey ) << tab
					       << jobspec->js_queued << tab
						   << jobspec->js_canceled << tab
						   << jobspec->js_L << tab
						   << jobspec->js_A << tab
						   << jobspec->js_sigma << tab
						   << jobspec->js_accuracy << tab
						   << jobspec->js_options << tab;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
					fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
			for( int i = 0; i < jobspec->js_options; i++ ){
				applesworkload << jobspec->js_option[i] << tab;
				if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
					fatal1( "could not write to \"%s\"", applesworkload_fn );
				}
			}
			applesworkload << endl;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
				fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
		}

		else
		if( jobspec->js_type == jst_nas ){
			applesworkload << benchname( jobspec->js_bench ) << tab
						   << classname( jobspec->js_class ) << tab
						   << jobspec->js_accuracy << endl;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
				fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
		}

	}

}



/**
 *  Write #jobspeclist# into the file #workload_fn#.#extension#, exposing all information known
 *  about the jobs.
 */

void writebushel( List<JobSpec>* jobspeclist, char* workload_fn, char* extension )
{

	char applesworkload_fn[ MAX_FILENAME + 1];
	strcpy( applesworkload_fn, workload_fn );
	strcat( applesworkload_fn, "." );
	strcat( applesworkload_fn, extension );

	ofstream applesworkload( applesworkload_fn );
	if( applesworkload == NULL ){
		fatal1( "could not open \"%s\"", applesworkload_fn );
	}

	const char tab = '\t';
	for( JobSpec* jobspec = jobspeclist->getfirst(); jobspec != NULL; jobspec = jobspeclist->getnext() ){

		applesworkload << jobspec->js_name << tab
					   << jobspec->js_owner << tab
					   << jobspec->js_tag << tab
					   << jobspectypename( jobspec->js_type ) << tab
					   << jobspec->js_queued << tab;
		if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
			fatal1( "could not write to \"%s\"", applesworkload_fn );
		}
		
		if( jobspec->js_type == jst_fixed ){
			applesworkload << jobspec->js_nodes << tab
						   << jobspec->js_reqtime << tab
						   << jobspec->js_start << tab
						   << jobspec->js_finish << tab
						   << jobspecstatusname( jobspec->js_status ) << endl;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
				fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
		}

		if( jobspec->js_type == jst_downey ){
			applesworkload << jobspec->js_canceled << tab
						   << jobspec->js_L << tab
						   << jobspec->js_A << tab
						   << jobspec->js_sigma << tab
						   << jobspec->js_accuracy << tab
						   << jobspec->js_options << tab;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
					fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
			for( int i = 0; i < jobspec->js_options; i++ ){
				applesworkload << jobspec->js_option[i] << tab;
				if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
					fatal1( "could not write to \"%s\"", applesworkload_fn );
				}
			}
			applesworkload << endl;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
				fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
		}

		if( jobspec->js_type == jst_nas ){
			applesworkload << benchname( jobspec->js_bench ) << tab
						   << classname( jobspec->js_class ) << tab
						   << jobspec->js_accuracy << endl;
			if( applesworkload.eof() || applesworkload.fail() || applesworkload.bad() ){
				fatal1( "could not write to \"%s\"", applesworkload_fn );
			}
		}

	}

}



/**
 *  For all possible #options# of downey jobs in #jobspeclist#, write the jobs into the 
 *  file #workload_fn#.#extension#$option$, making all jobs look static submissions
 */

void writenasoptions( List<JobSpec>* jobspeclist, int max_options, NasJobs* nasjobs, int availnodes, char* workload_fn, char* extension )
{

	char optionworkload_basefn[ MAX_FILENAME + 1];
	strcpy( optionworkload_basefn, workload_fn );
	strcat( optionworkload_basefn, "." );
	strcat( optionworkload_basefn, extension );

	for( int option = 0; option < max_options; option++ ){

		char optionworkload_fn[ MAX_FILENAME + 1];
		sprintf( optionworkload_fn, "%s%i", optionworkload_basefn, option );

		ofstream optionworkload( optionworkload_fn );
		if( optionworkload == NULL ){
			fatal1( "could not open \"%s\"", optionworkload_fn );
		}

		const char tab = '\t';
		for( JobSpec* jobspec = jobspeclist->getfirst(); jobspec != NULL; jobspec = jobspeclist->getnext() ){

			optionworkload << jobspec->js_name << tab
						   << jobspec->js_owner << tab
						   << jobspec->js_tag << tab
						   << jobspectypename( jst_fixed ) << tab
						   << jobspec->js_queued << tab;
			if( optionworkload.eof() || optionworkload.fail() || optionworkload.bad() ){
				fatal1( "could not write to \"%s\"", optionworkload_fn );
			}
		
			if( jobspec->js_type == jst_fixed ){
				optionworkload << jobspec->js_nodes << tab
							   << jobspec->js_reqtime << tab
							   << jobspec->js_start << tab
							   << jobspec->js_finish << tab
							   << jobspecstatusname( jobspec->js_status ) << endl;
				if( optionworkload.eof() || optionworkload.fail() || optionworkload.bad() ){
					fatal1( "could not write to \"%s\"", optionworkload_fn );
				}
			}

			if( jobspec->js_type == jst_nas ){
				int nodes;
				if( jobspec->js_bench == nb_EP ){
					int option_fraction = availnodes / max_options;
					nodes = 1 + option * option_fraction;
				}
				else {
					int feasibleoption = min( option, nasjobs->options( jobspec->js_bench, jobspec->js_class ) - 1 );
					nodes = nasjobs->option( jobspec->js_bench, jobspec->js_class, feasibleoption );
					if( nodes > availnodes ){
						nodes = nasjobs->option( jobspec->js_bench, jobspec->js_class, 0 );
					}
				}
				Time exectime = nasjobs->exectime( jobspec->js_bench, jobspec->js_class, nodes );
				Time reqtime = max( (Time) 1, (Time) ( exectime / jobspec->js_accuracy ) );
				Time finish = jobspec->js_start + exectime;
				optionworkload << nodes << tab
							   << reqtime << tab
							   << jobspec->js_start << tab
							   << finish << tab
							   << jobspecstatusname( jobspec->js_status ) << endl;
			}

			if( jobspec->js_type == jst_downey ){
				fatal( "a downey job present writenasoptions()" );
			}

		}

	}

}



/**
 *  Return the string name of #status#.
 */

char* jobspecstatusname( JobSpecStatus status )
{
	if( status == jss_not_used ){
		fatal( "a job with a non used status should not be printed out" );
	}
	static char* statusname[] = { "completed", "cancelled" };
	return statusname[(int) status];
}



/**
 *  Return the string name of #status#.
 */

char* jobspectypename( JobSpecType type )
{
	static char* typenames[] = { "fixed", "downey", "nas" };
	return typenames[(int) type];
}
