#ifndef MESON_DECAYER_H
#define MESON_DECAYER_H

// RooBarb
#include "IObject.h"


// ROOT
#include "TLorentzVector.h"
#include "TRandom.h"

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
	ParticleDecayer() {}
	
	ParticleDecayer( ParticleLibrary &_plcLib, FunctionLibrary &_funLib ) {
		this->plcLib = _plcLib;
		this->funLib = _funLib;
	}
	
	/*ParticleDecayer Dtor
	 *
	 */
	~ParticleDecayer() {}


	void setDecayChannel( DecayChannelInfo &_dcInfo ){

		this->dcInfo = _dcInfo;
		this->parent = this->plcLib.get( this->dcInfo.parentName );
		for ( string prod : this->dcInfo.productNames ){
			ParticleInfo plcInfo = plcLib.get( prod );
			products.push_back( plcInfo );
		}
	}

	string toString( int nTabs = 0){
		string str = "";
		string ind = indentation( nTabs );

		str += ind + "[ParticleDecayer]\n";
		str += ind + "DecayChannel: " + dcInfo.name + "\n";
		str += ind + "[Parent] " + parent.toString() + "\n";
		for ( ParticleInfo &pi : products ){
			str += ind + "[daughter] " + pi.toString() + "\n";
		}
		return str;
	}

	virtual void decay( TLorentzVector _lv ){
		
		DEBUG( classname(), "lv( P=" << dts(_lv.Px()) << "," << dts(_lv.Py()) << "," << dts(_lv.Pz()) << ", M=" << dts( _lv.M() ) << ")" );

		if ( products.size() == 2 ){
			DEBUG( classname(), "Two Body Decay" );
			if ( products[0].mass != products[1].mass ){
				ERROR( classname(), "Two Body Decay requested for particles of different mass" );
			} else {
				twoBodyDecay( _lv );
			}

		}
	}
	
protected:
	FunctionLibrary funLib;
	ParticleLibrary plcLib;

	DecayChannelInfo dcInfo;


	ParticleInfo parent;
	vector<ParticleInfo> products;



	virtual void applyBoost( TLorentzVector &_parent_lv, TLorentzVector &_d_lv ){

		float betaX = _parent_lv.Px() / _parent_lv.E();
		float betaY = _parent_lv.Py() / _parent_lv.E();
		float betaZ = _parent_lv.Pz() / _parent_lv.E();

		_d_lv.Boost(betaX,betaY,betaZ);
	}

	/* Two Body Decay in the rest frame of the parent
	 *
	 * ONLY Valid for m1 == m2! The daughter mass must be equal for this simplified form.
	 * The decay is computed and then the daughters are boosted into the frame of the parent
	 */
	virtual void twoBodyDecay( TLorentzVector _parent_lv ){
		DEBUG( classname(), "lv( P=" << dts(_parent_lv.Px()) << "," << dts(_parent_lv.Py()) << "," << dts(_parent_lv.Pz()) << ", M=" << dts( _parent_lv.M() ) << ")" );


		ParticleInfo &d1 = products[0];
		ParticleInfo &d2 = products[1];

		// MUST BE EQUAL
		double M_d1 = d1.mass;
		double M_d2 = d2.mass;


		double E_d = _parent_lv.M()/2.;
		double p = sqrt(E_d*E_d - M_d1*M_d1);

		double costheta = gRandom->Uniform(-1.,1.);
		double phi = gRandom->Uniform(0,TMath::Pi()*2);

		// make sure that the magnitude of the mom vector is correct!
		// May allow these distributions to be input?
		double pz = p*costheta;
		double px = p*sqrt(1.-costheta*costheta)*cos(phi);
		double py = p*sqrt(1.-costheta*costheta)*sin(phi);

		TLorentzVector daughter1( px, py, pz, E_d);
		TLorentzVector daughter2( -px, -py, -pz, E_d );

		applyBoost(_parent_lv,daughter1);
		applyBoost(_parent_lv,daughter2);

		// Note this is slightly different than Bingchu/Shuai's code
		// That code gets d2's P correct but mass wrong
		// this method gets the entire 4-vector correct
		d1.lv = daughter1;
		d2.lv = daughter2;
	}

	/* Dalitz Decay
	 *
	 */
	virtual void dalitzDecay( TLorentzVector _parent_lv ){

	}


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