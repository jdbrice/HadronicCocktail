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
#include "CintFunctionLibrary.h"

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

		this->pdfPhi      = this->funLib.get( "decay_product_phi" );
		this->pdfCosTheta = this->funLib.get( "decay_product_costheta" );

		if ( nullptr == this->pdfPhi )
			this->pdfPhi = shared_ptr<TF1>( new TF1( "decay_product_phi", "1 * ( x > 0 && x < 2.0 * TMath::Pi() )" ) );
		if ( nullptr == this->pdfCosTheta )
			this->pdfCosTheta = shared_ptr<TF1>( new TF1( "decay_product_costheta", " 1 * ( x >= -1 && x <= 1 )" ) );
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

		this->makeMassDistribution();

		if ( this->products.size() >= 3 )
			this->makeDileptonMassDistribution();
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

		// parent_lv.M() == pdgMass
		if ( _parent_lv.M() - n.mass > _M_ll && _M_ll > ( l1.mass + l2.mass ) )
			return true;
		return false;
	}

	virtual void decay( TLorentzVector _lv ){
		
		DEBUG( classname(), "lv( P=" << dts(_lv.Px()) << "," << dts(_lv.Py()) << "," << dts(_lv.Pz()) << ", M=" << dts( _lv.M() ) << ")" );
		int nProducs = products.size();
		if ( nProducs == 2 ){
			DEBUG( classname(), "Two Body Decay" );
			if ( products[0].mass != products[1].mass ){
				ERROR( classname(), "Two Body Decay requested for particles of different mass" );
			} else {

				// sample the parent mass and set the lv
				_lv.SetXYZM(  _lv.Px(), _lv.Py(), _lv.Pz(), sampleParentMass() );
				lastSampledMass = _lv.M();
				twoBodyDecay( _lv );
			}
		} else if ( nProducs == 3 ){

			_lv.SetXYZM(  _lv.Px(), _lv.Py(), _lv.Pz(), parent.mass );
			double M_ll = sampleDileptonMass();

			int iCatch = 0;
			while ( false == checkDalitzEnergyConservation( _lv, M_ll ) && iCatch < 1000 ){
				M_ll = sampleDileptonMass();				
				iCatch++;
			}

			if ( iCatch >= 999 ){
				ERROR( classname(), "Unable to meet Energy Conservation criteria, bailing out of decay" );
				return;
			}
			lastSampledMass = M_ll;
			dalitzDecay( _lv, M_ll );
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

	double getSampledMass(){
		return lastSampledMass;
	}

	int getNProducts(){
		return products.size();
	}
	
protected:
	FunctionLibrary funLib;
	ParticleLibrary plcLib;

	DecayChannelInfo dcInfo;


	ParticleInfo parent;
	vector<ParticleInfo> products;

	// parent particle's mass distribution
	shared_ptr<TF1> massDistribution = nullptr;
	// used in dalitz decays only
	shared_ptr<TF1> dileptonMassDistribution = nullptr;

	// alias to PDFs used in plc decays
	shared_ptr<TF1> pdfCosTheta;
	shared_ptr<TF1> pdfPhi;
	double lastSampledMass;


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


	double sampleParentMass(){
		if ( nullptr == massDistribution )
			return 0;

		return massDistribution->GetRandom();
	}

	double sampleDileptonMass(){
		if ( nullptr == dileptonMassDistribution )
			return 0;

		return dileptonMassDistribution->GetRandom();
	}


	virtual void applyBoost( TLorentzVector &_parent_lv, TLorentzVector &_d_lv ){

		float betaX = _parent_lv.Px() / _parent_lv.E();
		float betaY = _parent_lv.Py() / _parent_lv.E();
		float betaZ = _parent_lv.Pz() / _parent_lv.E();

		_d_lv.Boost(betaX,betaY,betaZ);
	}

	void rotate(double pIn[3], double pOut[3], double _costheta, double _sintheta, double _cosphi, double _sinphi){
		pOut[0] = pIn[0] * _costheta * _cosphi-pIn[1] * _sinphi+pIn[2] * _sintheta * _cosphi;
		pOut[1] = pIn[0] * _costheta * _sinphi+pIn[1] * _cosphi+pIn[2] * _sintheta * _sinphi;
		pOut[2] = -1.0  * pIn[0] * _sintheta + pIn[2] * _costheta;
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
		double costheta = this->pdfCosTheta->GetRandom(); //gRandom->Uniform(-1.,1.);
		double phi = this->pdfPhi->GetRandom(); //gRandom->Uniform(0,TMath::Pi()*2);

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

		// Mass in the lorentz vector is the sampled mass not necessarily the pdg Mass
		// Shuai's code uses the pdg mass NOT the sampled mass! Is this correct
		double pdgMass = parent.mass;//_parent_lv.M();

		// two body decay of electrons in virtual photon's rest frame
		double l_E    = _M_ll / 2; // Energy of lepton
		double p1     = sqrt( (l_E + l1.mass ) * ( l_E - l1.mass ) );
		double beta2  = 1.0 - 4.0 * ( l1.mass * l1.mass ) / ( _M_ll * _M_ll );
		double lambda = beta2 / ( 2.0 - beta2 );

		DEBUG( classname(), "l_E = " << l_E );
		DEBUG( classname(), "p1 = " << p1 );
		DEBUG( classname(), "beta2 = " << beta2 );
		DEBUG( classname(), "lambda = " << lambda );

		double costheta = this->pdfCosTheta->GetRandom();//( 2.0 * gRandom->Rndm() ) - 1.0;
		
		// if the neutral partical hass mass == 0 ( ie a photon) then we need to take care of the polarization
		if ( n.isPhoton() ){
			while( ( 1.0 + lambda * costheta * costheta ) < ( 2.0 * gRandom->Rndm() ) ){
				costheta = this->pdfCosTheta->GetRandom();
			}
		}

		double sintheta = sqrt( ( 1.0 + costheta ) * (1.0 - costheta) );
		double phi      = this->pdfPhi->GetRandom();
		double sinphi   = sin( phi );
		double cosphi   = cos( phi );

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

		costheta  = this->pdfCosTheta->GetRandom();//( 2.0 * gRandom->Rndm() ) - 1.;
		sintheta  = sqrt( (1.0 + costheta ) * ( 1.0 - costheta ) );
		phi       = this->pdfPhi->GetRandom();
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


	void makeMassDistribution(){
		INFO( classname(), "Making mass distribution for " << this->parent.toString() );
		string fname = this->parent.name + "_mass";
		
		// If the function library has the mass distribution then use it. If not build it as we think it should be
		if ( this->funLib.get( fname ) ){
			massDistribution = this->funLib.get( fname );
			massDistribution->SetRange( 0, 5 ); // TODO: make configurable
			massDistribution->SetNpx(10000);
			INFO( classname(), "Loaded the mass distribution for " << parent.name << " from the function library" );
			return;
		}


		// we use a special form for rho
		if ( "rho" == this->parent.name ){
			// TODO: add support for rho meson
			ERROR( classname(), "Rho Meson is not yet supported, you can add support by providing rho_mass to FunctionLibrary" );
		} else {
			// use a breit wigner shape
			massDistribution = shared_ptr<TF1>( new TF1( fname.c_str(), BreitWigner, 0, 10, 2 ) );
			
			// Set the BreitWigner to use the width and mass of this plc
			massDistribution->SetParameter( 0, this->parent.width );
			massDistribution->SetParameter( 1, this->parent.mass );

			massDistribution->SetRange( this->parent.mass - 10 * this->parent.width, this->parent.mass + 10 * this->parent.width ); // TODO: make configurable
			massDistribution->SetNpx(50000);

			massDistribution->Write();
		}
	} // makeMassDistribution

	void makeDileptonMassDistribution(){
		
		INFO( classname(), "Making mass distribution for " << this->parent.toString() );
		string fname = this->parent.name + "_massDilepton";
		
		// If the function library has the mass distribution then use it. If not build it as we think it should be
		if ( this->funLib.get( fname ) ){
			dileptonMassDistribution = this->funLib.get( fname );
			
			// if we load from config this should be set
			// dileptonMassDistribution->SetRange( 0, 5 ); // TODO: make configurable
			dileptonMassDistribution->SetNpx(50000);	// TODO: make configurable
			dileptonMassDistribution->Write();
			INFO( classname(), "Loaded the dilepton mass distribution for " << parent.name << " from the function library (ie Overrode default)" );
			return;
		}


		dileptonMassDistribution = shared_ptr< TF1 >( new TF1( fname.c_str(), KrollWada, 0, 10, 6 ) );
		

		// double KrollWada( double Mll, double M0, double Mn, double ml, double G0, double iL2, double Nd );
		dileptonMassDistribution->SetParameter( 0, this->parent.mass );
		dileptonMassDistribution->SetParameter( 1, this->neutral().mass );
		dileptonMassDistribution->SetParameter( 2, this->lepton1().mass );
		dileptonMassDistribution->SetParameter( 3, this->parent.gamma2 );
		dileptonMassDistribution->SetParameter( 4, this->parent.invLambda2 );
		dileptonMassDistribution->SetParameter( 5, this->parent.Nd );

		dileptonMassDistribution->SetRange( 0, 5 ); // TODO: make configurable
		dileptonMassDistribution->SetNpx(50000);
		dileptonMassDistribution->Write();


		// DEBUG ONLY
		if ( Logger::getGlobalLogLevel() >= Logger::llDebug ){

			TF1 * fPS = new TF1( (this->parent.name + "_PS" ).c_str(), PhaseSpaceMassive, 0, 10, 2 );
			fPS->SetRange( 0, 5 );
			fPS->SetParameters( this->parent.mass, this->neutral().mass );
			fPS->SetNpx( 50000 );
			fPS->Write();

			double psm  = PhaseSpaceMassive( 0.75, this->parent.mass, this->neutral().mass );
			DEBUG( classname(), "PS( 0.75, " << this->parent.mass << ", " << this->neutral().mass << " ) = " << psm );

			TF1 * fQED = new TF1( (this->parent.name + "_QED" ).c_str(), QED, 0, 10, 2 );
			fQED->SetRange( 0.0, 5 );
			DEBUG( classname(), "QED( " << this->lepton1().mass << ", " << this->parent.Nd <<") @ (0.22) = " << QED( 0.22, this->lepton1().mass, this->parent.Nd ) );

			fQED->SetParameters( this->lepton1().mass, this->parent.Nd );
			fQED->SetNpx( 50000 );
			fQED->Write();

			DEBUG( classname(), "FormFactor2( 0.72, " << this->parent.gamma2 << ", " << this->parent.invLambda2 << " ) = " << FormFactor2( 0.72, this->parent.gamma2, this->parent.invLambda2 ) );
			TF1 * fFF = new TF1( (this->parent.name + "_FF" ).c_str(), FormFactor2, 0, 5, 2 );
			fFF->SetRange( 0, 5 );
			fFF->SetParameters( this->parent.gamma2, this->parent.invLambda2 );
			fFF->SetNpx( 50000 );
			fFF->Write();

		}


	} //makeDileptonMassDistribution


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