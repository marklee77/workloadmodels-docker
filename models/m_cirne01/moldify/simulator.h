/**
 *  sss - Space Shared Scheduler
 *  simulator
 */



#ifndef __SIMULATOR_H
#define __SIMULATOR_H



#include "list.h"
#include "event.h"


#ifdef __SIMULATOR_CPP
	List<Event>* g_elist;
#else
	extern List<Event>* g_elist;
#endif


#endif