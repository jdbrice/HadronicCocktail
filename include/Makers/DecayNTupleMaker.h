#ifndef DECAY_NTUPLE_MAKER_H
#define DECAY_NTUPLE_MAKER_H

// Project
#include "Makers/DecayMaker.h"

// ROOT
#include "TFile.h"
#include "TNtuple.h"

class DecayNTupleMaker : public DecayMaker
{
protected:
	TNtuple *ntuple;
public:
	virtual const char* classname() const { return "DecayNTupleMaker"; }
	DecayNTupleMaker() {}
	~DecayNTupleMaker() {}

	virtual void initialize();

	static string ntupleSchema;
	static string ntupleName;


protected:
	virtual void postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd );
	virtual void postMake();
	
};


#endif