/**
 *  sss - Space Shared Scheduler
 *  Job
 */



#include <stdio.h>
#include <stdlib.h>

#define __JOB_C
#include "job.h"
#include "sss.h"


/**
 *  JobList's Invariants:
 *    - The list is ordered by j_submission
 *    - There is no ready job whose start time is less than vclock.now().
 **/


/**
 *  Add the job described by its parameters to the reservation list. 
 *  The job status is always s_ready.
 */

Job* JobList::add( int jobid, Time start, int requests, Request* request, int currentrequest, char* tag, CallBackFunc starthandler, CallBackFunc interrupthandler, void* handlerparam, SSS* sss, double reljobcount, double relload, Time schedlength )
{

	// create the list elem
	JobElem* new_elem = new JobElem( jobid, vclock.now(), start, requests, request, currentrequest, s_ready, tag, starthandler, interrupthandler, handlerparam, sss, reljobcount, relload, schedlength );

	// empty list
	if( jl_list == NULL ){
		jl_list = new_elem;
		jl_last = new_elem;
	}

	// non-empy list
	else {
		new_elem->je_prev = jl_last;
		new_elem->je_next = NULL;
		jl_last->je_next = new_elem;
		jl_last = new_elem;
	}

	return new_elem;

}


/**
 *  Return the job cooresponding to #jobid#. This object should not be directly altered by the caller.
 */

Job* JobList::find( int jobid )
{

	// empty list
	if( jl_list == NULL ){
		return NULL;
	}

	// non-empty list
	JobElem* job_elem = jl_list;
	while( job_elem->je_next != NULL && job_elem->j_jobid != jobid ){
			job_elem = job_elem->je_next;
	}
	if( job_elem->j_jobid == jobid ){
		return job_elem;
	}
	else {
		return NULL;
	}

}


/**
 *  Returns a runnable job. If such a job does not exist, it returns NULL.
 */

Job* JobList::runnable( Time now )
{

	// empty list
	if( jl_list == NULL ){
		return NULL;
	}

	// non-empty list
	JobElem* job_elem = jl_list;
	while( true ){
		if( job_elem->j_status == s_ready && job_elem->j_start < now ){
			fatal2( "job %d was supposed to have started running at %d", job_elem->getid(), job_elem->getstart() );
		}
		if( job_elem->j_status == s_ready && job_elem->j_start == now ){
			return job_elem;
		}
		if( job_elem->je_next == NULL ){
			return NULL;
		}
		job_elem = job_elem->je_next;
	};
	
}


Job* JobList::getfirst( Status status )
{
	jl_current = jl_list;
	return getnext( status );
}


Job* JobList::getnext( Status status )
{

	// empty list
	if( jl_current == NULL ){
		return NULL;
	}

	// non-empty list
	while( jl_current->je_next != NULL && jl_current->j_status != status ){
			jl_current = jl_current->je_next;
	}
	Job* result;
	if( jl_current->j_status == status ){
		result = jl_current;
	}
	else {
		result = NULL;
	}
	jl_current = jl_current->je_next;
	return result;

}


/**
 *  Update #job#'s start time and used request.
 */

void JobList::update( Job* job, Time start, int currentrequest )
{

	job->j_start = start;
	job->j_currentrequest = currentrequest;
}


/**
 *  Prints all jobs whose status equals #status# and returns how many nodes these jobs
 *  take in conjuction.
 */

int JobList::print( Status status )
{
	char tab = '\t';
//	cout << "externalid" << tab 
//		 << "jobid" << tab 
//		 << "submission" << tab 
//		 << "nodes" << tab 
//		 << "reqtime" << tab 
//		 << "start" << tab 
//		 << "finished" << tab 
//		 << "status" << tab 
//		 << "tag" << tab
//		 << "rel-job-count" << tab
//		 << "rel-load" << endl;
	int totalnodes = 0;
	for( JobElem* jobelem = jl_list; jobelem != NULL; jobelem = jobelem->je_next ){
		if( status == s_all || status == jobelem->j_status ){
			cout << jobelem->j_handlerparam << tab
				 << jobelem->j_jobid << tab
				 << jobelem->j_submission << tab 
				 << jobelem->getnodes() << tab 
				 << jobelem->getreqtime() << tab 
				 << jobelem->j_start << tab 
				 << jobelem->j_finished << tab 
				 << StatusName[jobelem->j_status] << tab 
				 << jobelem->j_tag << tab
				 << jobelem->j_reljobcount << tab
				 << jobelem->j_relload << tab
				 << jobelem->j_schedlength << endl;
			totalnodes += jobelem->getnodes();
		}
	}
#	ifdef __DEBUG
		cout << "total of nodes == " << totalnodes << endl;
#	endif
	return totalnodes;
}


/**
 *  Return how many jobs in the list have status equal #status#.
 */

int JobList::howmanyjobs( Status status )
{
	int result = 0;
	for( JobElem* jobelem = jl_list; jobelem != NULL; jobelem = jobelem->je_next ){
		if( jobelem->j_status == status ){
			result++;
		}
	}
	return result;
}



