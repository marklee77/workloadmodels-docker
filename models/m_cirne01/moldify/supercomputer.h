/**
 *  sss - Space Shared Scheduler
 *  SuperComputer
 */



#ifndef __SUPERCOMPUTER_H
#define __SUPERCOMPUTER_H



class SuperComputer;
#include "jobspec.h"
#include "sss.h"
#include "nasjobs.h"


class SuperComputer {
public:

	char* sc_name;
	SSS* sc_sss;
	NasJobs* sc_nasjobs;
	List<JobSpec>* sc_jobspeclist;

	SuperComputer() {
		sc_name = NULL;
		sc_sss = NULL;
		sc_nasjobs = NULL;
		sc_jobspeclist = new List<JobSpec>;
	}
	~SuperComputer() {
		delete sc_jobspeclist;
	}

};




#endif
