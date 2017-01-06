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

		ccol.setDefault( "pT", 0, 100000 );
		ccol.setDefault( "eta", -10, 10 );
		ccol.setDefault( "phi", -10, 10 );
		ccol.setDefault( "y", -10, 10 );

		ccol.report();

		pT  = ccol["pT"];
		eta = ccol["eta"];
		y   = ccol["y"];
		phi = ccol["phi"];
	}

	bool pass( TLorentzVector &lv, TH1D *_h = nullptr  ){
		// INFO( classname(), "lv( Pt=" << dts(lv.Pt()) << ",Eta=" << dts(lv.Eta()) << ",Phi" << dts(lv.Phi()) << ", M=" << dts( lv.M() ) << ")" );

		if ( nullptr != _h ){
			_h->Fill( 0 );
		}

		// do this first so that TLorentzVector::Eta does not complaine when pT==0
		if ( !pT->inInclusiveRange( lv.Pt() ) )
			return false;

		if ( nullptr != _h ){ _h->Fill( 1 ); }

		if ( !eta->inInclusiveRange( lv.Eta() ) ) return false;
		if ( nullptr != _h ){ _h->Fill( 2 ); }

		if ( !y->inInclusiveRange( lv.Rapidity() ) ) return false;
		if ( nullptr != _h ){ _h->Fill( 3 ); }

		if ( !phi->inInclusiveRange( lv.Phi() ) )  return false;
		if ( nullptr != _h ){ _h->Fill( 4 ); }

		return true;
	}

	bool fail( TLorentzVector &lv, TH1D *_h = nullptr ){
		return !pass( lv, _h );
	}

protected:

	CutCollection ccol;
	// alias
	shared_ptr<XmlRange> pT;
	shared_ptr<XmlRange> eta;
	shared_ptr<XmlRange> y;
	shared_ptr<XmlRange> phi;
	
};


#endif