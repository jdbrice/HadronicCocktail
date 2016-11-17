#ifndef UT_PARTICLE_SAMPLER_H
#define UT_PARTICLE_SAMPLER_H


// RooBarb
#include "TaskRunner.h"
#include "UnitTest/UnitTest.h"

#include "ParticleSampler.h"

class UnitTestParticleSampler : public TaskRunner
{
public:
	virtual const char* classname() const { return "UnitTestParticleSampler"; }
	UnitTestParticleSampler(){}
	~UnitTestParticleSampler(){}


	virtual void initialize(){
		
	}



	virtual void make(){

		UnitTest::start( config[ nodePath + ":subject" ] );

		ParticleSampler pSampler;
		





		INFO( classname(), UnitTest::summary() );

		config.toXmlFile( "UnitTestParticleSampler_out.xml" );
	}
	
};


#endif