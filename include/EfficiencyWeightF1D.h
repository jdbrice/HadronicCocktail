#ifndef EFFECIENCY_WEIGHT_F1D_H
#define EFFECIENCY_WEIGHT_F1D_H

#include "EfficiencyWeight.h"

#include "FunctionLibrary.h"

class EfficiencyWeightF1D : public EfficiencyWeight
{
protected:
	FunctionLibrary funLib;
	shared_ptr<TF1> fpT, feta, fphi;
public:
	const char* classname() const { return "EfficiencyWeightF1D"; }
	EfficiencyWeightF1D( FunctionLibrary &_funLib ) {
		this->funLib = _funLib;

		fpT  = this->funLib.get( "eff_pT" );
		feta = this->funLib.get( "eff_eta" );
		fphi = this->funLib.get( "eff_phi" );

	}
	~EfficiencyWeightF1D() {}
	
	virtual double weight( double pT, double eta, double phi, int charge = 1 ){
		if ( nullptr == fpT || nullptr == feta || nullptr == fphi )
			return 1.0;

		double w = fpT->Eval( pT );
		w       *= feta->Eval( eta );
		w       *= fphi->Eval( phi );

		return w;
	}

	virtual void writeDistributions(){
		if ( nullptr == fpT || nullptr == feta || nullptr == fphi )
			return;

		fpT->Write();
		feta->Write();
		fphi->Write();
	}

};

#endif