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

		products.clear();
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

	bool checkDalitzEnergyConservation( TLorentzVector _parent_lv, double _M_ll ){
		ParticleInfo &l1 = this->lepton1();
		ParticleInfo &l2 = this->lepton2();
		ParticleInfo &n  = this->neutral();

		if ( _parent_lv.M() - n.mass > _M_ll && _M_ll > ( l1.mass + l2.mass ) )
			return true;
		return false;
	}

	virtual void decay( TLorentzVector _lv, double _M_ll = 0.0 ){
		
		DEBUG( classname(), "lv( P=" << dts(_lv.Px()) << "," << dts(_lv.Py()) << "," << dts(_lv.Pz()) << ", M=" << dts( _lv.M() ) << ")" );
		int nProducs = products.size();
		if ( nProducs == 2 ){
			DEBUG( classname(), "Two Body Decay" );
			if ( products[0].mass != products[1].mass ){
				ERROR( classname(), "Two Body Decay requested for particles of different mass" );
			} else {
				twoBodyDecay( _lv );
			}
		} else if ( nProducs == 3 ){
			dalitzDecay( _lv, _M_ll );
		}
	}


	ParticleInfo getLepton1(){
		ParticleInfo ll1 = lepton1();
		return ll1;
	}

	ParticleInfo getLepton2(){
		ParticleInfo ll2 = lepton2();
		return ll2;
	}

	ParticleInfo getNeutral(){
		ParticleInfo n = neutral();
		return n;
	}
	
protected:
	FunctionLibrary funLib;
	ParticleLibrary plcLib;

	DecayChannelInfo dcInfo;


	ParticleInfo parent;
	vector<ParticleInfo> products;

	ParticleInfo &lepton1( ) {
		for ( ParticleInfo &pi : products ){
			if (pi.isLepton())
				return pi;
		}
		ERROR( classname(), "Cannot find lepton" );
		return products[0];
	}
	ParticleInfo &lepton2( ) {
		int nLeptons = 0;
		for ( ParticleInfo &pi : products ){
			if ( pi.isLepton() ){
				nLeptons++;
				if ( nLeptons == 2 ){
					return pi;
				}
			}
		}
		ERROR( classname(), "Cannot find two leptons" );
		return products[0];
	}

	ParticleInfo &neutral() {
		for ( ParticleInfo &pi : products ){
			if ( false == pi.isLepton() ){
				return pi;
			}
		}
		ERROR( classname(), "Cannot find non lepton" );
		return products[0];
	}



	virtual void applyBoost( TLorentzVector &_parent_lv, TLorentzVector &_d_lv ){

		float betaX = _parent_lv.Px() / _parent_lv.E();
		float betaY = _parent_lv.Py() / _parent_lv.E();
		float betaZ = _parent_lv.Pz() / _parent_lv.E();

		_d_lv.Boost(betaX,betaY,betaZ);
	}

	void rotate(double pin[3], double pout[3], double _costheta, double _sintheta, double _cosphi, double _sinphi)
	{
		pout[0] = pin[0] * _costheta * _cosphi-pin[1] * _sinphi+pin[2] * _sintheta * _cosphi;
		pout[1] = pin[0] * _costheta * _sinphi+pin[1] * _cosphi+pin[2] * _sintheta * _sinphi;
		pout[2] = -1.0  * pin[0] * _sintheta + pin[2] * _costheta;
		return;
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
	virtual void dalitzDecay( TLorentzVector _parent_lv, double _M_ll ){

		ParticleInfo &l1 = this->lepton1();
		ParticleInfo &l2 = this->lepton2();
		ParticleInfo &n  = this->neutral();

		double pdgMass = _parent_lv.M();

		// two body decay of elextrons in virtual photon's rest frame
		double l_E    = _M_ll / 2; // Energy of lepton
		double p1     = sqrt( (l_E + l1.mass ) * ( l_E - l1.mass ) );
		double beta2  = 1.0 - 4.0 * ( l1.mass * l1.mass ) / ( _M_ll * _M_ll );
		double lambda = beta2 / ( 2.0 - beta2 );

		DEBUG( classname(), "l_E = " << l_E );
		DEBUG( classname(), "p1 = " << p1 );
		DEBUG( classname(), "beta2 = " << beta2 );
		DEBUG( classname(), "lambda = " << lambda );

		double costheta = ( 2.0 * gRandom->Rndm() ) - 1.0;
		
		// if the neutral partical hass mass == 0 ( ie a photon) then we need to take care of the polarization
		if ( n.isPhoton() ){
			while( ( 1.0 + lambda * costheta * costheta ) < ( 2.0 * gRandom->Rndm() ) ){
				costheta = ( 2.0 * gRandom->Rndm() ) - 1.0;
			}
		}

		double sintheta = sqrt( ( 1.0 + costheta ) * (1.0 - costheta) );
		double phi = 2.0 * acos(-1.) * gRandom->Rndm();
		double sinphi = sin( phi );
		double cosphi = cos( phi );

		DEBUG( classname(), "costheta = " << costheta );
		DEBUG( classname(), "sintheta = " << sintheta );
		DEBUG( classname(), "phi = " << phi );
		DEBUG( classname(), "cosphi = " << cosphi );
		DEBUG( classname(), "sinphi = " << sinphi );

		// Store kinematics for letptons
		Double_t l1p[3] = {
			p1 * sintheta * cosphi, 
			p1 * sintheta * sinphi, 
			p1 * costheta
		};
		Double_t l2p[3] = {
			-1.0 * p1 * sintheta * cosphi, 
			-1.0 * p1 * sintheta * sinphi, 
			-1.0 * p1 * costheta
		};

		DEBUG( classname(), "P[0] = " << l1p[0] << ", " << l1p[1] << ", " << l1p[2] );
		DEBUG( classname(), "P[1] = " << l2p[0] << ", " << l2p[1] << ", " << l2p[2] );

		double E3 = ( pdgMass*pdgMass + n.mass * n.mass - _M_ll * _M_ll ) / ( 2.0 * pdgMass );
		double p3 = sqrt( (E3 + n.mass)  * (E3 - n.mass) );
		
		DEBUG( classname(), "E3 = " << E3 );
		DEBUG( classname(), "p3 = " << p3 );

		costheta  = ( 2.0 * gRandom->Rndm() ) - 1.;
		sintheta  = sqrt( (1.0 + costheta ) * ( 1.0 - costheta ) );
		phi       = 2.0 * acos(-1.0) * gRandom->Rndm();
		sinphi    = sin( phi );
		cosphi    = cos( phi );

		// Neutral particle's 4 vector in parent's rest frame
		n.lv.SetPxPyPzE( p3 * sintheta * cosphi, p3 * sintheta * sinphi, p3 * costheta, E3 );

		// lepton 4-vectors in the properly rotated virtual photon rest frame
		Double_t p1R[3] = {0.0};
		this->rotate(l1p, p1R, costheta, -sintheta, -cosphi, -sinphi);
		Double_t p2R[3] = {0.0};
		this->rotate(l2p, p2R, costheta, -sintheta, -cosphi, -sinphi); 

		DEBUG( classname(), "P[0] = " << p1R[0] << ", " << p1R[1] << ", " << p1R[2] );
		DEBUG( classname(), "P[1] = " << p2R[0] << ", " << p2R[1] << ", " << p2R[2] );

		l1.lv.SetPxPyPzE( p1R[0], p1R[1], p1R[2], l_E );
		l2.lv.SetPxPyPzE( p2R[0], p2R[1], p2R[2], l_E );

		// Now boost the leptons into the parent's rest frame
		double E_pll = sqrt( p3 * p3 + _M_ll * _M_ll );
		TVector3 pairBoost( -1.0 * n.lv.Px() / E_pll, -1.0 * n.lv.Py() / E_pll, -1.0 * n.lv.Pz() / E_pll );
		l1.lv.Boost( pairBoost );
		l2.lv.Boost( pairBoost );

		TVector3 labBoost( _parent_lv.Px() / _parent_lv.E(), _parent_lv.Py() / _parent_lv.E(), _parent_lv.Pz() / _parent_lv.E() );

		l1.lv.Boost( labBoost );
		l2.lv.Boost( labBoost );
		n.lv.Boost( labBoost );

		return;
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