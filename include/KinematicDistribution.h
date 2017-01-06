#ifndef KINEMATIC_DISTRIBUTION_H
#define KINEMATIC_DISTRIBUTION_H

// RooBarb
#include "IObject.h"
#include "XmlConfig.h"
#include "XmlFunction.h"
#include "XmlHistogram.h"
#include "Logger.h"
using namespace jdb;



/* Provides a union between multiple types of "distribution" backers
 *
 * Supplies a single interface to a distribution backed by one of multiple objects.
 * Currently supports TH1 and TF1 as backers.
 * 
 */
class KinematicDistribution : public IObject
{
public:
	virtual const char* classname() const { return "KinematicDistribution"; }
	KinematicDistribution() {}
	~KinematicDistribution() {}
	

	void set( shared_ptr<TH1> _h, shared_ptr<TF1> _f = nullptr ){
		if ( nullptr != _h )
			this->h = _h;
		if ( nullptr != _f ){
			this->f = _f;
		}
	}


	double sample(){

		// histograms come first since they are more likely to be user input 
		if ( nullptr != this->h ){
			return this->h->GetRandom();
		} else if ( nullptr != this->f ){
			return this->f->GetRandom();
		} 

		ERROR( classname(), "No valid backer for distribution" );
	}

	shared_ptr<TF1> getTF1(){
		return f;
	}


protected:

	shared_ptr<TH1> h = nullptr;
	shared_ptr<TF1> f = nullptr;


};



#endif