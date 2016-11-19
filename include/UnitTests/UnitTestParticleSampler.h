#ifndef UT_PARTICLE_SAMPLER_H
#define UT_PARTICLE_SAMPLER_H


// RooBarb
#include "TaskRunner.h"
#include "UnitTest/UnitTest.h"

#include "ParticleSampler.h"
#include "FunctionLibrary.h"
#include "HistogramLibrary.h"
#include "ParticleLibrary.h"

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
		FunctionLibrary funLib;
		HistogramLibrary histoLib;
		ParticleLibrary plcLib;
		plcLib.loadParticles( config, nodePath );
		funLib.loadAll( config, nodePath );
		histoLib.loadAll( config, nodePath );

		pSampler.set( plcLib.get( "omega" ), funLib, histoLib );


		TFile * f = new TFile( "UnitTestParticleSampler.root", "RECREATE" );
		TH1D * pt = new TH1D( "pT", "pT", 1000, 0, 10 );
		TH1D * eta = new TH1D( "eta", "eta", 1000, -2, 2 );
		TH1D * phi = new TH1D( "phi", "phi", 1000, -7, 7 );

		int N = config.getInt( nodePath + ".make:N" );
		for ( int i = 0; i < N; i++ ){
			TLorentzVector lv = pSampler.sample();
			cout << "lv( Pt=" << dts(lv.Pt()) << ",Eta=" << dts(lv.Eta()) << ",Phi" << dts(lv.Phi()) << ", M=" << dts( lv.M() ) << ")" << endl;

			pt->Fill( lv.Pt() );
			eta->Fill( lv.Eta() );
			phi->Fill( lv.Phi() );
		}


		f->Write();
		f->Close();

		INFO( classname(), UnitTest::summary() );

		config.toXmlFile( "UnitTestParticleSampler_out.xml" );
	}
	
};


#endif