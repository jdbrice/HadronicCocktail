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
	KinematicFilter 			  daughterFilterA;
	KinematicFilter 			  daughterFilterB;

	shared_ptr<TF1>				  momResolution;
	shared_ptr<TF1>				  momShape;
	
	bool momSmearing = false;
	bool makeQA = true;
	bool makeTF1 = false;
	bool makeHistos = true;

	CutCollection ccol;

	// RECO 4-vectors
	TLorentzVector rclv1, rclv2, rclv;

	shared_ptr<EfficiencyWeight> efficiency;
	double wEff = 1.0;

	bool keep_intermediate_states = true;
	bool keep_full_phase_space = true;

	double massDaughterA, massDaughterB;


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

	bool passDaughterFilters( TLorentzVector &rclv1, TLorentzVector &rclv2, string namedCut ){
		if ( daughterFilterA.pass( rclv1, namedCut ) && daughterFilterB.pass( rclv2, namedCut )  )
			return true;
		if ( daughterFilterB.pass( rclv1, namedCut ) && daughterFilterA.pass( rclv2, namedCut )  )
			return true;
		return false;
	}

	bool passDaughterFilters( TLorentzVector &rclv1, TLorentzVector &rclv2 ){
		if ( daughterFilterA.pass( rclv1 ) && daughterFilterB.pass( rclv2 )  )
			return true;
		if ( daughterFilterB.pass( rclv1 ) && daughterFilterA.pass( rclv2 )  )
			return true;
		return false;
	}

	unsigned long long int get_seed();

	virtual void make();
	virtual bool postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd );

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