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
#include "HistogramLibrary.h"
#include "ParticleInfo.h"

#include "KinematicDistribution.h"

class ParticleSampler : public IObject
{
public:
	virtual const char* classname() const { return "ParticleSampler"; }
	ParticleSampler() {}
	~ParticleSampler() {}

	void set( ParticleInfo _plcInfo, FunctionLibrary &_funLib, HistogramLibrary &_histoLib ){
		this->funLib   = _funLib;
		this->histoLib = _histoLib;
		this->plcInfo  = _plcInfo;

		// look for distributions
		
		kdPt.set( nullptr, this->funLib.get( "default_pT" ) ); // default in case nex ones are nullptr
		INFO( classname(), "Looking for TF1 " << quote( _plcInfo.name + "_pT" ) << " = " << this->funLib.get( _plcInfo.name + "_pT" ) << " exp : " << this->funLib.functionExpression( _plcInfo.name + "_pT" ) );
		kdPt.set( this->histoLib.get( _plcInfo.name + "_pT" ), this->funLib.get( _plcInfo.name + "_pT" ) );
		
		
		// set the particle's mass if given a TBW
		if ( "TsallisBlastWave" ==  this->funLib.functionExpression( _plcInfo.name + "_pT" ) ){
			INFO( classname(), "Setting TsallisBlastWave mass param to " << _plcInfo.mass );
			shared_ptr<TF1> f = this->funLib.get( _plcInfo.name + "_pT" );
			
			f->ReleaseParameter(0);
			f->SetParameter( 0, _plcInfo.mass );
			f->FixParameter( 0, _plcInfo.mass );
			INFO( classname(), "Checking mass is set to " <<  f->GetParameter( 0 ) << " [GeV/c]" );
		}


		kdRapidity.set( nullptr, this->funLib.get("CERES" ) );
		kdRapidity.getTF1()->SetParameter( 2, this->plcInfo.mass ); // set the plc mass in CERES parameterization
		kdRapidity.set( this->histoLib.get( _plcInfo.name + "_rapidity" ), this->funLib.get( _plcInfo.name + "_rapidity" ) );
		

		kdPhi.set( nullptr, this->funLib.get("Phi" ) );
		kdPhi.set( this->histoLib.get( _plcInfo.name + "_phi" ), this->funLib.get( _plcInfo.name + "_phi" ) );

		// after things have settled write out the functions, since these may be built as a double check
		kdPt.getTF1()->Write();
		kdRapidity.getTF1()->Write();
		kdPhi.getTF1()->Write();

		INFO( classname(), "Initialize the kinematic distributions" );
		kdPt.sample();
		kdRapidity.sample();
		kdPhi.sample();
		INFO( classname(), "Complete");

	}

	double samplePt(){
		return kdPt.sample();
	}

	double sampleRapidity(){
		return kdRapidity.sample();
	} 

	double samplePhi(){
		return kdPhi.sample();
	}

	double rapidityToEta( double _y, double _pT ){
		
		double m = this->plcInfo.mass;
		double mT = sqrt(_pT*_pT+m*m);
		double pZ = mT*TMath::SinH(_y);
		double pTot = sqrt(_pT*_pT+pZ*pZ);

		double eta = 0.5 * log( ( pTot+pZ )/(pTot-pZ));
		
		return eta;
	}

	TLorentzVector sample(){
		TLorentzVector lv;
		double pT = this->samplePt();
		double y = this->sampleRapidity();
		double phi = this->samplePhi();
		double eta = rapidityToEta( y, pT );

		lv.SetPtEtaPhiM( pT, eta, phi, this->plcInfo.mass );
		return lv;
	}

	ParticleInfo plc() const {
		return this->plcInfo;
	}

protected:
	ParticleInfo plcInfo;
	FunctionLibrary funLib;	
	HistogramLibrary histoLib;	

	KinematicDistribution kdPt, kdRapidity, kdPhi;
	

};


#endif