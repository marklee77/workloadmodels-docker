/**
 *  sss - Space Shared Scheduler
 *  To Do List:
 *    - Need to define the invariants and protect against unautorized changes
 *		For example, job->j_start = slot->getstart() in backfill()
 */


#ifndef __SSS_H
#include "sss.h"
#endif



#ifndef __JOB_H
#define __JOB_H


#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "clock.h"
#include "util.h"
#include "sss.h"


typedef struct {
	Time r_reqtime; 
	int r_nodes;
} Request;


typedef enum { s_ready, s_running, s_finished, s_canceled, s_interrupted, s_all } Status;
#ifdef __JOB_C
	char *StatusName[] = {
		"ready",
		"running",
		"finished",
		"canceled",
		"interrupted",
		"INVALID"
	};
#else
	extern char *StatusName[];
#endif


class Job;

typedef void (*CallBackFunc)( SSS* sss, Job* job, void* param );

class Job {

protected:

	// basic info
    int j_jobid;
	Time j_submission;

	int j_requests;
	Request* j_request;
	int j_currentrequest; 
    Time j_start;
	Time j_finished;
    Status j_status;
	char j_tag[MAX_NAME + 1];
	SSS* j_sss;
	
	// performance metrics
	double j_reljobcount;
	double j_relload;
	Time j_schedlength;

	// callbacks
	CallBackFunc j_starthandler;
	CallBackFunc j_interrupthandler;
	void* j_handlerparam;

	Job( int jobid, Time submission, Time start, int requests, Request* request, int currentrequest, Status status, char* tag, CallBackFunc starthandler, CallBackFunc interrupthandler, void* handlerparam, SSS* sss, double reljobcount, double relload, Time schedlength )
	{
		j_jobid = jobid;
		j_submission = submission;
		j_start = start;
		j_requests = requests;
		j_request = request;
		j_currentrequest = currentrequest;
		j_status = status;
		strcpy( j_tag, tag );
		j_starthandler = starthandler;
		j_interrupthandler = interrupthandler;
		j_handlerparam = handlerparam;
		j_reljobcount = reljobcount;
		j_relload = relload;
		j_schedlength = schedlength;
		j_sss = sss;
	}
	friend class JobList;

public:
	SSS* getsss(){
		return j_sss;
	}
	int getid(){
		return j_jobid;
	}
	Time getsubmission(){
		return j_submission;
	}
	Time getstart(){
		return j_start;
	}
	Time getreqtime(){
		return j_request[j_currentrequest].r_reqtime;
	}
	int getrequests(){
		return j_requests;
	}
	Request* getrequest(){
		return j_request;
	}
	int getcurrentrequest(){
		return j_currentrequest;
	}
	Time getexectime(){
		if( j_status != s_finished && j_status != s_canceled && j_status != s_interrupted ){
			fatal( "job::getexectime() called before the job terminates" );
		}
		if( j_finished > j_start ){
			return j_finished - j_start;
		}
		else {
			return 0;
		}
	}
	Time getwaitingtime(){
		if( j_status == s_ready ){
			fatal( "job::getwaitingtime() called before the job starts" );
		}
		return j_start - j_submission;
	}
	Time getturnaroundtime(){
		if( j_status != s_finished && j_status != s_canceled && j_status != s_interrupted ){
			fatal( "job::getturnaroundtime() called before the job terminates" );
		}
		return j_finished - j_submission;
	}
	int getnodes(){
		return j_request[j_currentrequest].r_nodes;
	}
	void setstatus( Status status ){
		j_status = status;
	}
	Status getstatus(){
		return j_status;
	}
	char* gettag(){
		return j_tag;
	}
	void setfinished( Time now ){
		j_finished = now;
	}
	void invokestarthandler(){
		(*j_starthandler)( j_sss, this, j_handlerparam );
	}
	void invokeinterrupthandler(){
		(*j_interrupthandler)( j_sss, this, j_handlerparam );
	}

};


class JobElem : public Job {
protected:
    JobElem* je_next;
    JobElem* je_prev;
    JobElem( int jobid, Time submission, Time start, int requests, Request* request, int currentrequest, Status status, char* tag, CallBackFunc starthandler, CallBackFunc interrupthandler, void* handlerparam, SSS* sss, double reljobcount, double relload, Time schedlength, JobElem* next = NULL, JobElem* prev = NULL ):
		Job( jobid, submission, start, requests, request, currentrequest, status, tag, starthandler, interrupthandler, handlerparam, sss, reljobcount, relload, schedlength )
	{
        je_next = NULL;
        je_prev = NULL;
    }
	JobElem& operator=( const Job& job ){
		Job* thisjob = (Job*) this;
		*thisjob = job;
		return *this;
	}
	friend class JobList;
};


class JobList {
    JobElem* jl_list;
	JobElem* jl_current;
	JobElem* jl_last;
public:
    JobList(){
        jl_list = NULL;
		jl_current = NULL;
		jl_last = NULL;
    }
	Job* add( int jobid, Time start, int requests, Request* request, int currentrequest, char* tag, CallBackFunc starthandler, CallBackFunc interrupthandler, void* handlerparam, SSS* sss, double reljobcount, double relload, Time schedlength );
    Job* find( int jobid );
	Job* runnable( Time now );
    Job* getfirst( Status status );
    Job* getnext( Status status );
	void update( Job* job, Time start, int currentrequest );
	int howmanyjobs( Status status );
	int print( Status status );
};


#endif
