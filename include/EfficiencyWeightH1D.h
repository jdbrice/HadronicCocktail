#ifndef EFFECIENCY_WEIGHT_H1D_H
#define EFFECIENCY_WEIGHT_H1D_H

#include "IObject.h"
using namespace jdb;

#include "HistogramLibrary.h"

class EfficiencyWeightH1D : public IObject
{
public:
	const char* classname() const { return "EfficiencyWeightH1D"; } 
	EfficiencyWeightH1D( HistogramLibrary &_histoLib ) {}
	~EfficiencyWeightH1D() {}

	virtual double weight( double pT, double eta, double phi, int charge = 1 ){
		return 1.0;
	}

	virtual void writeDistributions(){
		return;
	}
	
};

#endif