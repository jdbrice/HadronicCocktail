

// RooBarb
#include "XmlConfig.h"
#include "TaskEngine.h"
using namespace jdb;

// STL
#include <iostream>
#include <exception>


// CocktailMakers
#include "CocktailMakers/Scaler.h"
#include "CocktailMakers/CocktailMaker.h"
#include "CocktailMakers/CocktailNTupleMaker.h"

#include "CocktailMakers/Blender.h"
#include "CocktailMakers/ConvertCCbar.h"


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


	// CocktailMakers
	TaskFactory::registerTaskRunner<CocktailMaker>( "CocktailMaker" );	
	TaskFactory::registerTaskRunner<CocktailNTupleMaker>( "CocktailNTupleMaker" );	

	TaskFactory::registerTaskRunner<Blender>( "Blender" );
	TaskFactory::registerTaskRunner<ConvertCCbar>( "ConvertCCbar" );

	TaskFactory::registerTaskRunner<Scaler>( "Scaler" );

	TaskEngine engine( argc, argv );

	return 0;
}
