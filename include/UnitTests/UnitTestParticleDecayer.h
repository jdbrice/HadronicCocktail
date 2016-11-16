#ifndef UT_PARTICLE_DECAYER_INFO_H
#define UT_PARTICLE_DECAYER_INFO_H


// RooBarb
#include "TaskRunner.h"
#include "UnitTest/UnitTest.h"

#include "ParticleDecayer.h"

class UnitTestParticleDecayer : public TaskRunner
{
public:
	virtual const char* classname() const { return "UnitTestParticleDecayer"; }
	UnitTestParticleDecayer(){}
	~UnitTestParticleDecayer(){}


	virtual void initialize(){
		
	}



	virtual void make(){

		UnitTest::start( config[ nodePath + ":subject" ] );


		FunctionLibrary funLib;
		// funLib.loadAll( config, nodePath + ".FunctionLibrary" );
		ParticleLibrary plcLib;
		plcLib.loadParticles( config, nodePath + ".ParticleLibrary" );

		ParticleDecayer pd( plcLib, funLib );
		DecayChannelInfo dci( config, nodePath + ".DecayChannels.DecayChannel[1]" );

		pd.setDecayChannel( dci );

		INFO( classname(), "\n" << pd.toString(1) );

		TLorentzVector lv;
		lv.SetXYZM( 10.0, 1.0, 0.0, 1.2 );
		pd.decay( lv );

		INFO( classname(), "\n" << pd.toString(1) );

		INFO( classname(), UnitTest::summary() );

		config.toXmlFile( "UnitTestParticleDecayer_out.xml" );
	}
	
};


#endif