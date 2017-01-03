#ifndef KINEMATIC_FILTER_H
#define KINEMATIC_FILTER_H

#include "IObject.h"
#include "XmlConfig.h"
#include "CutCollection.h"
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

	bool pass( TLorentzVector &lv ){
		// INFO( classname(), "lv( Pt=" << dts(lv.Pt()) << ",Eta=" << dts(lv.Eta()) << ",Phi" << dts(lv.Phi()) << ", M=" << dts( lv.M() ) << ")" );

		// do this first so that TLorentzVector::Eta does not complaine when pT==0
		if ( !pT->inInclusiveRange( lv.Pt() ) )
			return false;

		if ( 	!eta->inInclusiveRange( lv.Eta() ) ||
				!y->inInclusiveRange( lv.Rapidity() ) ||
				!phi->inInclusiveRange( lv.Phi() ) )
			return false;
		return true;
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