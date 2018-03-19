#ifndef DECAY_MAKER_H
#define DECAY_MAKER_H

// RooBarb
#include "TaskRunner.h"
#include "IHistoBookMaker.h"
using namespace jdb;

// STL
#include <vector>
#include <string>
using namespace std;

// Project
#include "DecayChannelInfo.h"
#include "FunctionLibrary.h"
#include "ParticleLibrary.h"
#include "HistogramLibrary.h"
#include "ParticleDecayer.h"
#include "ParticleSampler.h"
#include "KinematicFilter.h"
#include "EfficiencyWeight.h"


// ROOT
#include "TNamed.h"

class DecayMaker : public TaskRunner, public IHistoBookMaker
{
protected:

	FunctionLibrary funLib;
	ParticleLibrary plcLib;
	HistogramLibrary histoLib;

	vector<string> activeChannels;
	map<string, DecayChannelInfo> namedDecayInfo;
	map<string, ParticleDecayer>  namedPlcDecayers;
	map<string, ParticleSampler>  namedPlcSamplers;

	map<string, int>              namedN;
	map<string, double>           namedWeight;

	int maxN;

	KinematicFilter 			  parentFilter;
	KinematicFilter 			  daughterFilter;

	shared_ptr<TF1>				  momResolution;
	shared_ptr<TF1>				  momShape;
	
	bool momSmearing = false;
	bool makeQA = true;
	bool makeTF1 = false;

	CutCollection ccol;

	// RECO 4-vectors
	TLorentzVector rl1lv, rl2lv, rplv;

	shared_ptr<EfficiencyWeight> efficiency;
	double wEff = 1.0;

public:
	virtual const char* classname() const { return "DecayMaker"; }
	DecayMaker() {}
	~DecayMaker() {}

	virtual void init( XmlConfig &_config, string _nodePath ){
		TaskRunner::init( _config, _nodePath );
		initialize();
	}

	virtual void initialize();

	virtual void fillState( string _s, 
							TLorentzVector &_lvMc, TLorentzVector &_lvRc, 
							TLorentzVector &_lvMc1, TLorentzVector &_lvRc1, 
							TLorentzVector &_lvMc2, TLorentzVector &_lvRc2,
							double _w = 1.0 );

protected:

	virtual void make();
	virtual void postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd );

	virtual void postMake() {
		TaskRunner::postMake();

		TNamed n( "config", config.toXml() );
		n.Write();
	}

	virtual void overrideConfig() {
		DEBUG( classname(), "" );

		if ( !config.exists( "jobIndex" ) || config.getInt( "jobIndex" ) == -1 ){
			config.set( "jobIndex", "all" );
		}
	}

	void prepareQAHistos();

	void prepareHistos();
	
};


#endif