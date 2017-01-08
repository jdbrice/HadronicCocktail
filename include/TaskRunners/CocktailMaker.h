#ifndef COCKTAIL_MAKER_H
#define COCKTAIL_MAKER_H

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


class CocktailMaker : public TaskRunner, public IHistoBookMaker
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

	bool makeQA = true;
	bool makeTF1 = false;

	// RECO 4-vectors
	TLorentzVector rl1lv, rl2lv, rplv;

public:
	virtual const char* classname() const { return "CocktailMaker"; }
	CocktailMaker() {}
	~CocktailMaker() {}

	virtual void init( XmlConfig &_config, string _nodePath ){
		TaskRunner::init( _config, _nodePath );
		initialize();
	}

	virtual void initialize();

protected:

	virtual void make();
	virtual void postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd );

	virtual void overrideConfig() {
		DEBUG( classname(), "" );

		if ( !config.exists( "jobIndex" ) || config.getInt( "jobIndex" ) == -1 ){
			config.set( "jobIndex", "all" );
		}
	}
	
};


#endif