#ifndef PARTICLE_SAMPLER_H
#define PARTICLE_SAMPLER_H


// RooBarb
#include "IObject.h"


// ROOT
#include "TLorentzVector.h"
#include "TRandom.h"
#include "TRandom3.h"

// Project
#include "ParticleLibrary.h"
#include "FunctionLibrary.h"
#include "ParticleInfo.h"

class ParticleSampler : public IObject
{
public:
	virtual const char* classname() const { return "ParticleSampler"; }
	ParticleSampler() {}
	ParticleSampler( ParticleInfo _parent ) {

	}
	~ParticleSampler() {}

	void set( ParticleInfo _plcInfo, FunctionLibrary &_funLib ){
		this->funLib = _funLib;
		this->plcInfo = _plcInfo;
		this->makeMassDistribution();
		this->makeDileptonMassDistribution();
	}

	double sampleMass(){

	}

protected:
	ParticleInfo plcInfo;
	FunctionLibrary funLib;	
	
	// particles mass distribution
	shared_ptr<TF1> massDistribution = nullptr;

	// used in dalitz decays only
	shared_ptr<TF1> dileptonMassDistribution = nullptr;


	void makeMassDistribution(){
		INFO( classname(), "Making mass distribution for " << this->plcInfo.toString() );
		string fname = this->plcInfo.name + "_mass";
		
		// If the function library has the mass distribution then use it. If not build it as we think it should be
		if ( this->funLib.get( fname ) ){
			massDistribution = this->funLib.get( fname );
			massDistribution->SetRange( 0, 5 ); // TODO: make configurable
			massDistribution->SetNpx(10000);
			INFO( classname(), "Loaded the mass distribution for " << plcInfo.name << " from the function library" );
			return;
		}


		// we use a special form for rho
		if ( "rho" == this->plcInfo.name ){

		} else {
			// use a breit wigner shape
			massDistribution = shared_ptr<TF1>( new TF1( fname.c_str(), BreitWigner, 0, 10, 2 ) );
			
			// Set the BreitWigner to use the width and mass of this plc
			massDistribution->SetParameter( 0, this->plcInfo.width );
			massDistribution->SetParameter( 1, this->plcInfo.mass );

			massDistribution->SetRange( 0, 5 ); // TODO: make configurable
			massDistribution->SetNpx(10000);
		}

	}

	void makeDileptonMassDistribution(){

	}
};


#endif