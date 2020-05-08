#ifndef UT_PARTICLE_LIBRARY_H
#define UT_PARTICLE_LIBRARY_H


// RooBarb
#include "TaskRunner.h"
#include "UnitTest.h"

#include "ParticleLibrary.h"

class UnitTestParticleLibrary : public TaskRunner
{
public:
	virtual const char* classname() const { return "UnitTestParticleLibrary"; }
	UnitTestParticleLibrary(){}
	~UnitTestParticleLibrary(){}


	virtual void initialize(){
		
	}



	virtual void make(){

		UnitTest::start( config[ nodePath + ":subject" ] );

		ParticleLibrary plib;
		plib.loadParticles( config, nodePath );

		// Test the known particles to check the loading of MC id
		UT_TEST(  "MC ID: pi0",      UT_EQ( plib.get( "pi0" )      .mcId, 111 ) );
		UT_TEST(  "MC ID: eta",      UT_EQ( plib.get( "eta" )      .mcId, 221 ) );
		UT_TEST(  "MC ID: rho",      UT_EQ( plib.get( "rho" )      .mcId, 113 ) );
		UT_TEST(  "MC ID: omega",    UT_EQ( plib.get( "omega" )    .mcId, 223 ) );
		UT_TEST(  "MC ID: phi",      UT_EQ( plib.get( "phi" )      .mcId, 333 ) );
		UT_TEST(  "MC ID: etaprime", UT_EQ( plib.get( "etaprime" ) .mcId, 331 ) );
		UT_TEST(  "MC ID: jpsi",     UT_EQ( plib.get( "jpsi" )     .mcId, 443 ) );
		UT_TEST(  "MC ID: psi",      UT_EQ( plib.get( "psi" )      .mcId, 100443 ) );


		UT_TEST(  "MASS: pi0",      UT_EQ( plib.get( "pi0" )      .mass, 0.1349766 ) );
		UT_TEST(  "MASS: eta",      UT_EQ( plib.get( "eta" )      .mass, 0.547853 ) );
		UT_TEST(  "MASS: rho",      UT_EQ( plib.get( "rho" )      .mass, 0.77549 ) );
		UT_TEST(  "MASS: omega",    UT_EQ( plib.get( "omega" )    .mass, 0.78265 ) );
		UT_TEST(  "MASS: phi",      UT_EQ( plib.get( "phi" )      .mass, 1.019455 ) );
		UT_TEST(  "MASS: etaprime", UT_EQ( plib.get( "etaprime" ) .mass, 0.95778 ) );
		UT_TEST(  "MASS: jpsi",     UT_EQ( plib.get( "jpsi" )     .mass, 3.096916 ) );
		UT_TEST(  "MASS: psi",      UT_EQ( plib.get( "psi" )      .mass, 3.68609 ) );


		UT_TEST(  "WIDTH: pi0",      UT_DEQ( plib.get( "pi0" )     .width, 0.0, 0.0001 ) );
		UT_TEST(  "WIDTH: eta",      UT_EQ( plib.get( "eta" )      .width, 1.3e-6 ) );
		UT_TEST(  "WIDTH: rho",      UT_EQ( plib.get( "rho" )      .width, 0.1491 ) );
		UT_TEST(  "WIDTH: omega",    UT_EQ( plib.get( "omega" )    .width, 8.49e-3 ) );
		UT_TEST(  "WIDTH: phi",      UT_EQ( plib.get( "phi" )      .width, 4.26e-3 ) );
		UT_TEST(  "WIDTH: etaprime", UT_EQ( plib.get( "etaprime" ) .width, 1.94e-4 ) );
		UT_TEST(  "WIDTH: jpsi",     UT_EQ( plib.get( "jpsi" )     .width, 9.29e-5 ) );
		UT_TEST(  "WIDTH: psi",      UT_EQ( plib.get( "psi" )      .width, 3.04e-4 ) );


		INFO( classname(), UnitTest::summary() );

		config.toXmlFile( "UnitTestParticleLibrary_out.xml" );
	}
	
};


#endif