/*****
 *
 *  parameters for a stream of jobs
 *  
 */



#ifndef __JOBSTREAM_H
#define __JOBSTREAM_H


#include "clock.h"


typedef enum { ap_anl, ap_ctc, ap_kth, ap_sdsc } ArrivalPattern;


class JobStream {
	// characteristics of the job stream
	double js_k;    
	int js_reductionfactor;
	ArrivalPattern js_ap;
	int js_maxnodes;
	double js_scalefactor;
	// keep state for the arrival generator
	bool js_initarrival;
	int js_arrivalday;
	double js_arrivalnorm;
	double js_arrivalacculrate;
public:
	JobStream( ArrivalPattern ap, int maxnodes, double k, int reductionfactor ){
		js_ap = ap;
		js_maxnodes = maxnodes;
		js_k = k;
		js_reductionfactor = reductionfactor;
		if( js_reductionfactor > 2 ){
			fatal( "not prepared to deal with rection factor greater than 2" );
		}

		int orignodes;
		switch( ap ){
		case ap_anl: orignodes = 120;
					 break;
		case ap_ctc: orignodes = 430;
					 break;
		case ap_kth: orignodes = 100;
					 break;
		case ap_sdsc: orignodes = 128;
 					  break;
		}
		js_scalefactor = ((double) maxnodes) / ((double) orignodes);
		js_initarrival = true;
	};
	Time arrival();
	int nodes();
	Time reqtime();
	double accuracy();
	bool cancelled();
	Time cancellag();
};


#endif
