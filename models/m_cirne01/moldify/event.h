/**
 *  sss - Space Shared sssheduler
 *  Event
 */


#ifndef __EVENT_H
#define __EVENT_H


#include "job.h"
#include "jobspec.h"
#include "sss.h"


typedef enum { et_submit, et_cancel, et_finish, et_interrupt } EventType;


class Event {
public:

	Time e_time;
	EventType e_type;
	Job* e_job;
	JobSpec* e_jobspec;
	SSS* e_sss;

	Event( Time time, EventType type, JobSpec* jobspec, SSS* sss )
	{
		if( type != et_submit ){
			fatal1( "creating an invalid event at time %ld", time );
		}
		e_time = time;
		e_type = type;
		e_job = NULL;
		e_jobspec = jobspec;
		e_sss = sss;
	}
	Event( Time time, EventType type, Job* job, SSS* sss )
	{
		if( type != et_cancel && type != et_finish && type != et_interrupt ){
			fatal1( "creating an invalid event at time %ld", time );
		}
		e_time = time;
		e_type = type;
		e_job = job;
		e_jobspec = NULL;
		e_sss = sss;
	}

	// to support List<Event>
	Time id() {
		return e_time;
	}
	Time rank() {
		return e_time;
	}

};


#endif

