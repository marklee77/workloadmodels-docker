/**
 *  sss - Space Shared Scheduler
 *  AppLeS
 */



#ifndef __APPLES_H
#define __APPLES_H



#include "job.h"
#include "jobspec.h"
#include "sss.h"
#include "nasjobs.h"


void apples( JobSpec* jobspec, SSS** scs, int lastsc, CallBackFunc starthandler, CallBackFunc interrupthandler );



#endif
