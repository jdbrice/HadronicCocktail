

// RooBarb
#include "XmlConfig.h"
#include "TaskEngine.h"
using namespace jdb;

// STL
#include <iostream>
#include <exception>


// TaskRunners
#include "MesonDecayMaker.h"

int main( int argc, char* argv[] ) {

	TaskFactory::registerTaskRunner<MesonDecayMaker>( "MesonDecayMaker" );

	TaskEngine engine( argc, argv );

	return 0;
}
