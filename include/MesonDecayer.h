#ifndef MESON_DECAYER_H
#define MESON_DECAYER_H

// RooBarb
#include "IObject.h"

// ROOT
#include "TLorentzVector.h"

/* MesonDecayer
 *
 */
class MesonDecayer : public IObject
{
public:
	virtual const char* classname() const { return "MesonDecayer"; }
	
	/*MesonDecayer Ctor
	 *
	 */
	MesonDecayer() {}
	
	/*MesonDecayer Dtor
	 *
	 */
	~MesonDecayer() {}
	

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