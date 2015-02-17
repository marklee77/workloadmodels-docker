/**
 *  sss - Space Shared Scheduler
 *  Clock
 */


#ifndef __CLOCK_H
#define __CLOCK_H


#include <limits.h>

#include "util.h"


#define INFINITY ULONG_MAX


typedef unsigned long Time;

class VClock {
	Time c_now;
public:
	inline VClock() {
		c_now = 0;
	}
	inline Time now() {
		return c_now;
	}
	inline void settime( Time now ){
		if( now < c_now ){
			fatal( "clock moving backwards" );
		}
		c_now = now;
	}
};


#ifdef __CLOCK_CPP
	VClock vclock;
#else
    extern VClock vclock;
#endif


#endif
