#ifndef SINGLE_DECAY_MAKER_H
#define SINGLE_DECAY_MAKER_H


// RooBarb
#include "TaskRunner.h"
#include "Logger.h"
#include "Utils.h"

// ROOT

// Project
#include "ParticleSampler.h"
#include "FunctionLibrary.h"
#include "HistogramLibrary.h"
#include "ParticleLibrary.h"

/*
 *
 */
class SingleDecayMaker : public TaskRunner
{
public:
	virtual const char* classname() const { return "SingleDecayMaker"; }
	/*
	 *
	 */
	SingleDecayMaker(){}
	/*
	 *
	 */
	~SingleDecayMaker(){}

	/*
	 *
	 */
	void initialize(){

		plcLib.loadParticles( config, nodePath );
		funLib.loadAll( config, nodePath );
		histoLib.loadAll( config, nodePath );

		string parent = config.getXString( nodePath + ".make:parent" );
		pSampler.set( plcLib.get( parent ), funLib, histoLib );

	}


protected:

	ParticleSampler pSampler;
	FunctionLibrary funLib;
	HistogramLibrary histoLib;
	ParticleLibrary plcLib;



	virtual void make(){
		INFO( classname(), "" );

		int N = config.getInt( nodePath + ".make:N" );
		TaskProgress tp( "Generating Decays for : \n" << pSampler.plc().toString() , N );

		for ( int i = 0; i < N; i++ ){
			tp.showProgress( i );
			TLorentzVector lv = pSampler.sample();

			cout << "lv( Pt=" << dts(lv.Pt()) << ",Eta=" << dts(lv.Eta()) << ",Phi" << dts(lv.Phi()) << ", M=" << dts( lv.M() ) << ")" << endl;

			// pt->Fill( lv.Pt() );
			// eta->Fill( lv.Eta() );
			// phi->Fill( lv.Phi() );
		}

	}
	
};
#endif