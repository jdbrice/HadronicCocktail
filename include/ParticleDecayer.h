#ifndef MESON_DECAYER_H
#define MESON_DECAYER_H

// RooBarb
#include "IObject.h"


// ROOT
#include "TLorentzVector.h"

// Project
#include "ParticleLibrary.h"
#include "FunctionLibrary.h"

/* ParticleDecayer
 *
 */
class ParticleDecayer : public IObject
{
public:
	virtual const char* classname() const { return "ParticleDecayer"; }
	
	/*ParticleDecayer Ctor
	 *
	 */
	ParticleDecayer( ParticleLibrary &_plcLib, FunctionLibrary &_funLib ) {
		this->plcLib = _plcLib;
		this->funLib = _funLib;
	}
	
	/*ParticleDecayer Dtor
	 *
	 */
	~ParticleDecayer() {}


	void setDecayChannel( DecayChannelInfo _dcInfo ){

	}
	
protected:
	FunctionLibrary &funLib;
	ParticleLibrary &plcLib;

	DecayChannelInfo dcInfo;


	ParticleInfo parent;
	vector<ParticleInfo> prods;


	/*Calculates the phase space for a 2-body decay
	 *
	 */
	// static twoBodyDecay( ... );
	/*Calculates the phase space for a 3-body dalitz decay
	 *
	 */
	// static dalitzDecay( ... );

	/*Calculates boost from parent and applies to daughter
	 *
	 */
	// static TLorentzVector applyBoost( TLorentzVector parent, TLorentzVector daughter )


};

#endif