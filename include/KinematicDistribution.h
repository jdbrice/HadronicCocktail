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

	bool isReady() {
		return this->h != nullptr || this->f != nullptr;
	}

	double sample(){

		// histograms come first since they are more likely to be user input 
		if ( nullptr != this->h ){
			// INFO( classname(), "Sampling from TH1");
			return this->h->GetRandom();
		} else if ( nullptr != this->f ){
			// INFO( classname(), "Sampling from TF1");
			return this->f->GetRandom();
		} 

		ERROR( classname(), "No valid backer for distribution" );
		return 0.0;
	}

	shared_ptr<TF1> getTF1(){
		return f;
	}

	double eval( double x){
		if ( nullptr != this->f ){
			return this->f->Eval(x);
		} else if( nullptr != this->h ){
			int ibin = this->h->GetXaxis()->FindBin( x );
			return this->h->GetBinContent(ibin);
		}
		return std::numeric_limits<double>::quiet_NaN();
	}


protected:

	shared_ptr<TH1> h = nullptr;
	shared_ptr<TF1> f = nullptr;


};



#endif