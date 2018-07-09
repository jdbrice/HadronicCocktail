

// RooBarb
#include "XmlConfig.h"
#include "TaskEngine.h"
using namespace jdb;

// STL
#include <iostream>
#include <exception>


// Makers
#include "Makers/Scaler.h"
#include "Makers/DecayMaker.h"
#include "Makers/DecayNTupleMaker.h"
#include "Makers/HistogramMaker.h"

#include "Makers/Blender.h"
#include "Makers/ConvertCCbar.h"
#include "Makers/HistogramTBW.h"


// UnitTests
#include "UnitTestFunctionLibrary.h"
#include "UnitTestParticleLibrary.h"
#include "UnitTestDecayChannelInfo.h"
#include "UnitTestParticleDecayer.h"
#include "UnitTestParticleSampler.h"
#include "UnitTestHistogramLibrary.h"

int main( int argc, char* argv[] ) {

	// UnitTests
	TaskFactory::registerTaskRunner<UnitTestFunctionLibrary>( "UnitTestFunctionLibrary" );
	TaskFactory::registerTaskRunner<UnitTestParticleLibrary>( "UnitTestParticleLibrary" );
	TaskFactory::registerTaskRunner<UnitTestDecayChannelInfo>( "UnitTestDecayChannelInfo" );
	TaskFactory::registerTaskRunner<UnitTestParticleDecayer>( "UnitTestParticleDecayer" );
	TaskFactory::registerTaskRunner<UnitTestParticleSampler>( "UnitTestParticleSampler" );
	TaskFactory::registerTaskRunner<UnitTestHistogramLibrary>( "UnitTestHistogramLibrary" );


	// Makers
	TaskFactory::registerTaskRunner<DecayMaker>( "DecayMaker" );	
	TaskFactory::registerTaskRunner<DecayNTupleMaker>( "DecayNTupleMaker" );
	TaskFactory::registerTaskRunner<HistogramMaker>( "HistogramMaker" );

	TaskFactory::registerTaskRunner<Blender>( "Blender" );
	TaskFactory::registerTaskRunner<ConvertCCbar>( "ConvertCCbar" );

	TaskFactory::registerTaskRunner<HistogramTBW>( "HistogramTBW" );

	TaskFactory::registerTaskRunner<Scaler>( "Scaler" );

	TaskEngine engine( argc, argv );

	return 0;
}
