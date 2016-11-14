#ifndef MASS_DISTRIBUTION_H
#define MASS_DISTRIBUTION_H

// RooBarb
#include "IObject.h"
using namespace jdb;


// ROOT
#include "TF1.h"

// STL

// Project

class MassDistribution : public IObject
{
public:
	virtual const char* classname() const { return "MassDistribution"; }
	MassDistribution( double _mass, double width ) {
		// this->tf1BreitWigner = new TF1( "tf1_BreitWigner" )
	}
	~MassDistribution() {}

	double sampleBreitWignerMass() {
		return pdgMass;
	}


protected:
	double pdgMass;
	double pdgWidth;

};



#endif