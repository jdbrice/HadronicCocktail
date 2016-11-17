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
		massDistribution = shared_ptr<TF1>( new TF1( fname.c_str(), TF_BreitWigner, 0, 10, 2 ) );

		// Set the BreitWigner to use the width and mass of this plc
		massDistribution->SetParameter( 0, this->plcInfo.width );
		massDistribution->SetParameter( 1, this->plcInfo.mass );
	}

	void makeDileptonMassDistribution(){

	}
};


#endif