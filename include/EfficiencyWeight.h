#ifndef EFFECIENCY_WEIGHT_H
#define EFFECIENCY_WEIGHT_H

#include "IObject.h"
using namespace jdb;

class EfficiencyWeight : public IObject
{
public:
	const char* classname() const { return "EfficiencyWeight"; } 
	EfficiencyWeight() {}
	~EfficiencyWeight() {}

	virtual double weight( double pT, double eta, double phi, int charge = 1 ){
		return 1.0;
	}

	virtual void writeDistributions(){
		return;
	}
	
};

#endif