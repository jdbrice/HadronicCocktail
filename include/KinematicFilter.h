#ifndef KINEMATIC_FILTER_H
#define KINEMATIC_FILTER_H

#include "IObject.h"
#include "XmlConfig.h"
#include "CutCollection.h"
#include "HistoBook.h"
using namespace jdb;



class KinematicFilter : public IObject
{
public:
	virtual const char* classname() const { return "KinematicFilter"; }
	KinematicFilter() {}
	KinematicFilter( XmlConfig &_config, string _nodePath ) {
		load( _config, _nodePath );
	}
	~KinematicFilter() {}

	virtual void load( XmlConfig &_config, string _nodePath ){
		ccol.init( _config, _nodePath );

		ccol.setDefault( "leadingPt" , 0   , 100000 );
		ccol.setDefault( "pT"        , 0   , 100000 );
		ccol.setDefault( "eta"       , -10 , 10 );
		ccol.setDefault( "phi"       , -10 , 10 );
		ccol.setDefault( "y"         , -10 , 10 );

		ccol.report();

		leadingPt = ccol[ "leadingPt" ];
		pT        = ccol["pT"];
		eta       = ccol["eta"];
		y         = ccol["y"];
		phi       = ccol["phi"];
	}

	bool pass( TLorentzVector &lv, TH1D *_h = nullptr  ){
		// INFO( classname(), "lv( Pt=" << dts(lv.Pt()) << ",Eta=" << dts(lv.Eta()) << ",Phi" << dts(lv.Phi()) << ", M=" << dts( lv.M() ) << ")" );

		if ( nullptr != _h ){
			_h->Fill( 0 );
		}

		// do this first so that TLorentzVector::Eta does not complaine when pT==0
		if ( !pT->inInclusiveRange( lv.Pt() ) || lv.Pt() != lv.Pt() ){
			return false;
		}

		if ( nullptr != _h ){ _h->Fill( 1 ); }
		
		if ( !eta->inInclusiveRange( lv.Eta() ) ) {
			return false;
		}
		if ( nullptr != _h ){ _h->Fill( 2 ); }
		
		if ( !y->inInclusiveRange( lv.Rapidity() ) ){
			return false;
		} 

		
		if ( nullptr != _h ){ _h->Fill( 3 ); }

		if ( !phi->inInclusiveRange( lv.Phi() ) ) {
			return false;
		}

		if ( nullptr != _h ){ _h->Fill( 4 ); }

		// printf("passed rclv[pt=%0.2f, eta=%0.2f, phi=%0.2f, y=%0.2f, M=%0.2f]\n", lv.Pt(), lv.PseudoRapidity(), lv.Phi(), lv.Rapidity(), lv.M() );
		return true;
	}

	bool pass( TLorentzVector &lv, string name ){

		// do this first so that TLorentzVector::Eta does not complaine when pT==0
		if (lv.Pt() != lv.Pt()) return false;

		if ( "eta" == name ){
			if ( !eta->inInclusiveRange( lv.Eta() ) ) return false;
		} else if ( "pt" == name ){
			if ( !pT->inInclusiveRange( lv.Pt() ) ) return false;
		} else if ( "y" == name ){
			if ( !y->inInclusiveRange( lv.Rapidity() ) ) return false;
		} else if ( "phi" == name ){
			if ( !phi->inInclusiveRange( lv.Phi() ) )  return false;
		}
		return true;
	}

	bool fail( TLorentzVector &lv, TH1D *_h = nullptr ){
		return !pass( lv, _h );
	}

	bool pass( TLorentzVector &lv1, TLorentzVector &lv2 ){
		if ( !pass( lv1 ) ) return false;
		if ( !pass( lv2 ) ) return false;
		if ( !leadingPt->inInclusiveRange( lv1.Pt() ) && !leadingPt->inInclusiveRange( lv2.Pt() ) )
			return false;

		return true;
	}

	bool pass( TLorentzVector &lv1, TLorentzVector &lv2, string name ){
		if ( !pass( lv1, name ) ) return false;
		if ( !pass( lv2, name ) ) return false;
		
		if ( "pt" == name && !leadingPt->inInclusiveRange( lv1.Pt() ) && !leadingPt->inInclusiveRange( lv2.Pt() ) ) return false;

		return true;
	}

	bool fail( TLorentzVector &lv1, TLorentzVector &lv2 ){
		return !pass( lv1, lv2 );
	}

	double deltaY() {
		return y->max - y->min;
	}

	string toString(){
		string m = "";
		m += dts(y->min) +" < y <" + dts( y->max );
		return m;

	}

protected:

	CutCollection ccol;
	// alias
	shared_ptr<XmlRange> leadingPt;
	shared_ptr<XmlRange> pT;
	shared_ptr<XmlRange> eta;
	shared_ptr<XmlRange> y;
	shared_ptr<XmlRange> phi;

	
};


#endif