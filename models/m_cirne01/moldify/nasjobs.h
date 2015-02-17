/**
 *  sss - Space Shared Scheduler
 *  nasbench 
 **/


#ifndef __NASBENCH_H
#define __NASBENCH_H


#include "clock.h"


enum { nb_MG = 0, nb_LU, nb_SP, nb_BT, nb_EP };
typedef int NasBench;
const NasBench nb_first = nb_MG;
const NasBench nb_last = nb_EP;
char* benchname( NasBench nasbench );


enum { nc_B = 0, nc_C };
typedef int NasClass;
const NasClass nc_first = nc_B;
const NasClass nc_last = nc_C;
char* classname( NasClass nasclass );


class NasJobs {

	int nj_availnodes;

#	define NJ_MAX_OPTIONS 15
	int nj_options[nb_last+1][nc_last+1];
	int nj_nodestable[nb_last+1][nc_last+1][NJ_MAX_OPTIONS+1];
	Time nj_exectimetable[nb_last+1][nc_last+1][NJ_MAX_OPTIONS+1];

	void initexectimes( char* exectimes_fn );

	// to support getfirstoption() and getnextoption()
	NasBench nj_bench;
	NasClass nj_class;
	int nj_optionindex;
	int nj_epoption;

public:

	NasJobs( char* exectimes_fn, int availnodes );

	Time exectime( NasBench nasbench, NasClass nasclass, int nodes );

	int getfirstoption( NasBench nasbench, NasClass nasclass );
	int getnextoption();
	int genrandomoption( NasBench nasbench, NasClass nasclass );
	bool isanoption( NasBench nasbench, NasClass nasclass, int nodes );
	int options( NasBench nasbench, NasClass nasclass );
	int option( NasBench nasbench, NasClass nasclass, int option );

};


#endif

