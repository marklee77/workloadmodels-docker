/*****
 *
 *  main.cpp - test wstat
 *
 */



#include <fstream>
#include <iostream>

#include "util.h"
#include "random.h"


const long sample = 10000;


main()
{

	// set-up
	int i;
	char* data_fn;
	ofstream* data;
	StartRandom();

	// uniform
	data_fn = "uniform.data";
	data = new ofstream( data_fn );
	if( data == NULL ){
		fatal1( "could not open \"%s\"", data_fn );
	}
	for( i = 0; i < sample; i++ ){
		(*data) << UniformRandom( 1000, 10000 ) << endl;
	}
	delete data;

	// probalility
	data_fn = "prob.data";
	data = new ofstream( data_fn );
	if( data == NULL ){
		fatal1( "could not open \"%s\"", data_fn );
	}
	for( i = 0; i < sample; i++ ){
		(*data) << ProbRandom() << endl;
	}
	delete data;

	// normal
	data_fn = "normal.data";
	data = new ofstream( data_fn );
	if( data == NULL ){
		fatal1( "could not open \"%s\"", data_fn );
	}
	for( i = 0; i < sample; i++ ){
		(*data) << NormalRandom( 2, 5 ) << endl;
	}
	delete data;

	// gamma
	data_fn = "gamma.data";
	data = new ofstream( data_fn );
	if( data == NULL ){
		fatal1( "could not open \"%s\"", data_fn );
	}
	for( i = 0; i < sample; i++ ){
		(*data) << GammaRandom( 0.5, 5 ) << endl;
	}
	delete data;

	// uniform log
	data_fn = "unilog.data";
	data = new ofstream( data_fn );
	if( data == NULL ){
		fatal1( "could not open \"%s\"", data_fn );
	}
	for( i = 0; i < sample; i++ ){
		(*data) << UniLogRandom( 0.06, -0.2 ) << endl;
	}
	delete data;

	return 0;
}
