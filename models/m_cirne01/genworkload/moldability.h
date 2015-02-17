/**
 *  moldability model
 */


#ifndef _MOLDABILITY_H
#define _MOLDABILITY_H


typedef enum { psd_continous, psd_p2_bias, psd_job_nature } PartSizeDist;
	// psd_continous = follows a uniform log distribution
	// psd_p2_bias = follows the same distribution but biased towards the closed
	//               power of 2 with probability p_p
	// psd_job_nature = uses the constraint discovered in the survey

typedef enum { c_rigid, c_powerof2, c_square, c_even, c_multof4, c_unconstraint } CAlg;
const CAlg c_first = c_rigid;
const CAlg c_last = c_unconstraint;
char* constraintname( CAlg calg );

CAlg genconstraint( PartSizeDist partdistsize, int nodes );
int gencmin();
int gencu( CAlg calg );
double gensigma();
double genA( int cmin );
int genoptions( int cmin, CAlg calg, int cu, double A, double sigma, int availnodes, int* result );


#endif