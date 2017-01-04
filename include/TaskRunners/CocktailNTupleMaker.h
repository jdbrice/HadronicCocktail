#ifndef COCKTAIL_NTUPLE_MAKER_H
#define COCKTAIL_NTUPLE_MAKER_H

// Project
#include "TaskRunners/CocktailMaker.h"

// ROOT
#include "TFile.h"
#include "TNtuple.h"

class CocktailNTupleMaker : public CocktailMaker
{
protected:
	TNtuple *ntuple;
public:
	virtual const char* classname() const { return "CocktailNTupleMaker"; }
	CocktailNTupleMaker() {}
	~CocktailNTupleMaker() {}

	virtual void initialize();


protected:
	virtual void postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd );
	virtual void postMake();
	
};


#endif