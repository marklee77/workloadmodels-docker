/**
 *  sss - Space Shared Scheduler
 */



#ifndef __OCCUPANCY_H
#define __OCCUPANCY_H


#include <stdlib.h>

#include "clock.h"


class OccupancySlot {
protected:
    Time os_start;           /* in seconds (since Jan 1 1970) */
    Time os_duration;        /* in seconds */
    int os_nodes;            /* allocated nodes at this time slot */
    inline OccupancySlot( Time start, Time duration, int nodes ){
        os_start = start;
        os_duration = duration;
        os_nodes = nodes;
    }
	friend class Occupancy;
public:
	inline Time getstart(){
		return os_start;
	}
	inline Time getduration(){
		return os_duration;
	}
};


class OccupancyElem : public OccupancySlot {
protected:
    OccupancyElem* oe_prev;
    OccupancyElem* oe_next;
    OccupancyElem( Time start, Time duration, int nodes, OccupancyElem* prev = NULL, OccupancyElem* next = NULL ):
	    OccupancySlot( start, duration, nodes )
	{
        oe_prev = prev;
        oe_next = next;
    }
    void divide( Time duration1 );
    void coalesce();
	friend class Occupancy;
};


class Occupancy {

    int oc_nodes;
    OccupancyElem* oc_list;
    OccupancyElem* oc_current;

	unsigned long oc_incarnation;
	unsigned long oc_getfirst_incarnation;

    void compresslist();

public:

    Occupancy( int nodes );

    void allocate( Time start, Time duration, int nodes );
    void deallocate( Time start, Time duration, int nodes );

	Time length();

    OccupancySlot* getfirst( int nodes );
    OccupancySlot* getnext( int nodes );

	void print();

};


#endif

