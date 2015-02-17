/**
 *  sss - Space Shared Scheduler
 */


#ifndef __UTIL_H
#define __UTIL_H



// includes

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
using namespace std;
#include <string.h>
#ifdef AIX
#	include <strings.h>
#endif



// type and constant definitions

#ifdef WIN32
	typedef long longlong;
#else
	typedef long long longlong;
#endif

const int SUCCESS = 0;
const int FAILURE = 1;

const int MAX_NAME = 50;
typedef char Name[MAX_NAME + 1];
const int MAX_FILENAME = 150;
typedef char FileName[MAX_FILENAME + 1];
const int MAX_MSG = 200;
const int MAX_LINE = 600;


// erros and warnings

void fatal( char* msg );
#define fatal1( msg, param )  {									\
	char s[MAX_MSG + 1];									    \
	sprintf( s, (msg), (param) );								\
	fatal( s );													\
}				     
#define fatal2( msg, param1, param2 )  {						\
	char s[MAX_MSG + 1];									    \
	sprintf( s, (msg), (param1), (param2) );					\
	fatal( s );													\
}				     
#define fatal3( msg, param1, param2, param3 )  {				\
	char s[MAX_MSG + 1];									    \
	sprintf( s, (msg), (param1), (param2), (param3) );			\
	fatal( s );													\
}				     
void warning( char* msg );



// math functions

bool similar( int x, int y, int perc );

bool ispowerof2( int n );

double round( double d );

inline double logbase( double x, double y ){
	return log( x ) / log( y );
}

double polyeval( int degree, double coef[], double x );

double max( double x, double y );
long max( long x, long y );
int max( int x, int y );
unsigned long max( unsigned long x, unsigned long y );
unsigned max( unsigned x, unsigned y );
double min( double x, double y );
long min( long x, long y );
int min( int x, int y );
unsigned long min( unsigned long x, unsigned long y );
unsigned min( unsigned x, unsigned y );


// miscelaneous

#ifdef WIN32
#	define  strcasecmp( x, y )  _stricmp( (x), (y) )
#endif


void dirname( char* result, char* filename );
void basename( char* result, char* filename );


void shuffle( int* array, int size );


/**
 *  get the double preceeded by token #token# in the configuration file #conffile#.
 */

template <class ResultType, class StorageType>
ResultType getconf( char* conffilename, char* target )
{
	ifstream confstream( conffilename, ( ios::in ) );
	if( confstream == NULL ){
		fatal( "could not open the configuration file" );
	}

	char token[MAX_LINE + 1];
	do {
		confstream >> token;
		if( confstream.eof() || confstream.fail() || confstream.bad() ){
			fatal1( "problems reading conf named \"%s\"", conffilename );
		}
	} while( strcasecmp( token, target ) != 0 );

	static StorageType result;
	confstream >> result;
	if( confstream.eof() || confstream.fail() || confstream.bad() ){
		fatal1( "problems reading conf named \"%s\"", conffilename );
	}

	return result;
};



#endif
