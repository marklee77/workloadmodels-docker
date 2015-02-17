/**
 *  sss - Space Shared Scheduler
 *  Occupancy
 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <iostream.h>

#include "random.h"
#include "clock.h"
#include "util.h"



void fatal( char* msg )
{
    cerr << "sss: fatal error at " << vclock.now() << ": " << msg << endl;
//	getchar();
    exit( -1 );
}



void warning( char* msg )
{
    cerr << "sss: warning at " << vclock.now() << ": " << msg << endl;
//	getchar();
}



/**
 *  Are #x# and #y# within #perc# percent of each other?
 */

bool similar( int x, int y, int perc )
{
	if( x < y ){
		return ( ( 1.0 + ((double) perc)/100.0 ) * ((double) x) >= ((double) y) );
	}
	else {
		return ( ( 1.0 + ((double) perc)/100.0 ) * ((double) y) >= ((double) x) );
	}
}



/**
 *  Returns whether #n# is a power of 2.
 */

bool ispowerof2( int n )
{
	if( n > 2048 ){
		fatal( "the detection of powers of 2 has to be improved" );
	}
	return ( n == 1 || n == 2 || n == 4 || n == 8 || n == 16 || n == 32 || n == 64 || n == 128 || n == 256 || n == 512 || n == 1024 || n == 2048 );
}


/**
 *  Return #d# rounded to the closest integer
 */

double round( double d )
{
	if( fmod( d, 1.0 ) > 0.5 ){
		d++;
	}
	return floor( d );
}



/**
 *  Evaluates the polynomial of degree #degree# with coeficients #coef# at the point #x#.
 *  #coef#[0] is the coeficient of #x#^#degree#. #coef#[#degree#] is the constant. 
 */

double polyeval( int degree, double coef[], double x )
{
	double result = coef[0];
	for( int n = 1; n <= degree; n++ ){
		result = coef[n] + x * result;	
	}
	return result;
}



/**
 *  maximum
 */

double max( double x, double y )
{
	return (x > y)? x: y;
}

long max( long x, long y )
{
	return (x > y)? x: y;
}

int max( int x, int y )
{
	return (x > y)? x: y;
}

unsigned long max( unsigned long x, unsigned long y )
{
	return (x > y)? x: y;
}

unsigned max( unsigned x, unsigned y )
{
	return (x > y)? x: y;
}



/**
 *  minimum
 */

double min( double x, double y )
{
	return (x < y)? x: y;
}

long min( long x, long y )
{
	return (x < y)? x: y;
}

int min( int x, int y )
{
	return (x < y)? x: y;
}

unsigned long min( unsigned long x, unsigned long y )
{
	return (x < y)? x: y;
}

unsigned min( unsigned x, unsigned y )
{
	return (x < y)? x: y;
}



/**
 *  Puts in #result# the directory of #filename#
 */

void dirname( char* result, char* filename )
{
	strcpy( result, filename );
	
	char* end = result;
	char* p = result;
	while( *p ){
		if( *p == '/' ){
			end = p + 1;
		}
		p++;
	}
	*end = '\0';
}



/**
 *  Remove the directory of #filename# and puts remaining file in #result# 
 */

void basename( char* result, char* filename )
{
	char* start = filename;
	char* p = filename;
	while( *p ){
		if( *p == '/' ){
			start = p + 1;
		}
		p++;
	}

	strcpy( result, start );
}



/**
 *  Shuffle #array#, which is known to have #size# elements
 */

void shuffle( int* array, int size )
{
	for( int i = size - 1; i >= 0; i-- ){
		int j = UniformRandom( 0, i );
		int swap = array[i];
		array[i] = array[j];
		array[j] = swap;
	}
}