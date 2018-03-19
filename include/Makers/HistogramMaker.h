#ifndef HISTOGRAM_MAKER_H
#define HISTOGRAM_MAKER_H

#include "Makers/DecayMaker.h"


class HistogramMaker : public DecayMaker {
protected:
public:
	HistogramMaker() {}
	~HistogramMaker() {}

	virtual void initialize(){
		DecayMaker::initialize();
	}

	virtual void postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd ){
		
	}

protected:
};



#endif