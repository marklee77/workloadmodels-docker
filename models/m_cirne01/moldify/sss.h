/**
 *  sss - Space Shared Scheduler
 */


#ifndef __SSS_H
#define __SSS_H


class SSS;
#include "job.h"
#include "util.h"
#include "list.h"
#include "nasjobs.h"
#include "jobspec.h"
#include "occupancy.h"


typedef enum { s_backfilling, s_flooding, s_mjs } Scheduler;
inline Scheduler schedulerbyname( char* name ){
	if( strcasecmp( name, "backfilling" ) == 0 ){
		return s_backfilling;
	} else 
	if( strcasecmp( name, "flooding" ) == 0 ){
		return s_flooding;
	} else 
	if( strcasecmp( name, "mjs" ) == 0 ){
		return s_mjs;
	}  
	else {
		fatal1( "unknown scheduler name: %s", name );
	}
	return (Scheduler) 0;	// this gets rid of a warning
};


class SSS {

	// core
    int sss_nodes;
	Scheduler sss_scheduler;
    Occupancy* sss_occupancy;
    JobList* sss_joblist;
    int sss_next_jobid;

	// code methods
	int terminate( int jobid, Status status );
    void backfill();
    void start_runnable_jobs();
	int moldableschedule( int requests, Request* request, Time* p_start );
	Time schedule( int nodes, Time exectime );

	// current state
	int sss_jobcount;
	double reljobcount(){
		return ( (double) sss_jobcount ) / ( (double) sss_nodes );
	}
	longlong sss_load;
	double relload(){
		return ( (double) sss_load ) / ( (double) sss_nodes );
	}
	int sss_busynodes;

	// statistics
	Time sss_start;
	Time sss_last;
	double sss_utilization;
	double sss_mean_xfactor;
	double sss_mean_waiting_time;
	double sss_mean_turn_around_time;
	int sss_jobs_started;
	int sss_jobs_finished;
	void update_utilization();
	void update_on_submission( Job* job );
	void update_on_startup( Job* job );
	void update_on_termination( Job* job, bool everran );
	double currentutilization();

public:

	// basic info
	char* sss_name;
	NasJobs* sss_nasjobs;
	List<JobSpec>* sss_jobspeclist;

    SSS(){
		sss_name = NULL;
		sss_nodes = 0;
		sss_nasjobs = NULL;
		sss_jobspeclist = new List<JobSpec>;
    }

    SSS( char* name, Scheduler scheduler, int nodes, NasJobs* nasjobs ){

		sss_name = name;
		sss_scheduler = scheduler;
		sss_nodes = nodes;
		sss_nasjobs = nasjobs;
		sss_jobspeclist = new List<JobSpec>;

        sss_joblist = new JobList();
        sss_occupancy = new Occupancy( nodes );
        sss_next_jobid = 1;

		sss_jobcount = 0;
		sss_load = 0;
		sss_busynodes = 0;

		sss_start = 0;
		sss_utilization = 0;
		sss_mean_xfactor = 0;
		sss_mean_waiting_time = 0;
		sss_mean_turn_around_time = 0;
		sss_jobs_started = 0;
		sss_jobs_finished = 0;

    }
	
    Job* submit( int requests, Request* request, char* tag, CallBackFunc starthandler, CallBackFunc interrupthandler, void* handlerparam );
	int cancel( int jobid );
    int finished( int jobid );
    int interrupt( int jobid );
	Job* query( int jobid );

	int getnodes(){
		return sss_nodes;
	}
	Occupancy* getoccupancy(){
		return sss_occupancy;
	}
	Scheduler getscheduler(){
		return sss_scheduler;
	}

	double getutilization() { 
		return sss_utilization;
	}
	double getmeanxfactor() {
		return sss_mean_xfactor;
	}
	double getmeanwaitingtime() {
		return sss_mean_waiting_time;
	}
	double getmeanturnaroundtime() {
		return sss_mean_turn_around_time;
	}

	int howmanyjobs( Status status ){
		return sss_joblist->howmanyjobs( status );
	}
	void print( Status status ){
		sss_joblist->print( status );
	}
	void printoccupancy(){
		sss_occupancy->print();
	}
};



#endif
