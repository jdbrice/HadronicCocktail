

// RooBarb
#include "XmlConfig.h"
#include "TaskEngine.h"
using namespace jdb;

// STL
#include <iostream>
#include <exception>


// TaskRunners
// #include "MesonDecayMaker.h"

// UnitTests
#include "UnitTestFunctionLibrary.h"
#include "UnitTestParticleLibrary.h"
#include "UnitTestDecayChannelInfo.h"

int main( int argc, char* argv[] ) {

	// UnitTests
	TaskFactory::registerTaskRunner<UnitTestFunctionLibrary>( "UnitTestFunctionLibrary" );
	TaskFactory::registerTaskRunner<UnitTestParticleLibrary>( "UnitTestParticleLibrary" );
	TaskFactory::registerTaskRunner<UnitTestDecayChannelInfo>( "UnitTestDecayChannelInfo" );

	TaskEngine engine( argc, argv );

	return 0;
}
