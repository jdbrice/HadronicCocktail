#ifndef MESON_DECAY_MAKER_H
#define MESON_DECAY_MAKER_H


// RooBarb
#include "TaskRunner.h"
#include "Logger.h"
#include "Utils.h"

// ROOT

// Project
#include "VectorMeson.h"
#include "MesonDecayer.h"

/*
 *
 */
class MesonDecayMaker : public TaskRunner
{
public:
	virtual const char* classname() const { return "MesonDecayMaker"; }
	/*
	 *
	 */
	MesonDecayMaker(){}
	/*
	 *
	 */
	~MesonDecayMaker(){}

	/*
	 *
	 */
	void initialize(){}


protected:


	virtual void make(){
		INFO( classname(), "" );

		VectorMeson vm( config, nodePath );


		int N = config.getInt( nodePath + ":N", 1000 );

		TaskProgress tp( "Generating Decays", N );
		for ( int i = 0; i < N; i++ ){
			tp.showProgress( i );


			// vm.decay();


		}
	}
	
};
#endif