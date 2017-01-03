#ifndef COCKTAIL_NTUPLE_MAKER_H
#define COCKTAIL_NTUPLE_MAKER_H

// Project
#include "TaskRunners/CocktailMaker.h"

// ROOT
#include "TFile.h"
#include "TNtuple.h"

class CocktailNTupleMaker : public CocktailMaker
{
protected:
	// TFile   *file;
	TNtuple *ntuple;
public:
	virtual const char* classname() const { return "CocktailNTupleMaker"; }
	CocktailNTupleMaker() {}
	~CocktailNTupleMaker() {}

	virtual void initialize(){
		CocktailMaker::initialize();

		// open the TFile
		// file = new TFile( config.getXString( nodePath + ".output.TFile:url" ).c_str(), "RECREATE" );
		// setup the ntuple
		book->cd();
		ntuple = new TNtuple( "llCocktail", "Dilepton cocktail tuple format",
			"pPdgM:pPt:pEta:pPhi:pM:l1PtMc:l1PtRc:l1Eta:l1Phi:l1M:l2PtMc:l2PtRc:l2Eta:l2Phi:l2M:nPtMc:nEta:nPhi:nM:Mll" );
		// parent pdg Mass
		// parent 4-vector
		// lepton1 Pt MC
		// lepton1 4-vector (with RECO Pt)
		// lepton2 Pt MC
		// lepton2 4-vector (with RECO Pt)
		// neutral 4-vector (if available)
		// Reco dilepton Mass 
	}


protected:
	virtual void postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd ){
		CocktailMaker::postDecay( _name, _parent, _pd );

		TLorentzVector l1lv = _pd.getLepton1().lv;
		TLorentzVector l2lv = _pd.getLepton2().lv;
		

		Float_t data[19] = {0};

		data[0]  = _parent.M();		    // parent PDG mass
		data[1]  = _parent.Pt();		// parent 4 - vector with sampled mass
		data[2]  = _parent.Eta();
		data[3]  = _parent.Phi();
		data[4]  = _pd.getSampledMass();

		data[5]  = l1lv.Pt();
		data[6]  = rl1lv.Pt();
		data[7]  = l1lv.Eta();
		data[8]  = l1lv.Phi();
		data[9]  = l1lv.M();

		data[10] = l2lv.Pt();
		data[11] = rl2lv.Pt();
		data[12] = l2lv.Eta();
		data[13] = l2lv.Phi();
		data[14] = l2lv.M();

		if ( 3 == _pd.getNProducts() ){
			TLorentzVector nlv  = _pd.getNeutral().lv;
			data[15] = nlv.Pt();	
			data[16] = nlv.Eta();
			data[17] = nlv.Phi();
			data[18] = nlv.M();
		}

		data[19] = rplv.M();


		ntuple->Fill( data );

	}

	virtual void postMake(){
		INFO( classname(), "Closing the file and NTuple" );
	}
	
};


#endif