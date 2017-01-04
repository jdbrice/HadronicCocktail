#include "TaskRunners/CocktailNTupleMaker.h"


void CocktailNTupleMaker::initialize(){
	CocktailMaker::initialize();

	// open the TFile
	// file = new TFile( config.getXString( nodePath + ".output.TFile:url" ).c_str(), "RECREATE" );
	// setup the ntuple
	book->cd();
	ntuple = new TNtuple( "llCocktail", "Dilepton cocktail tuple format",
		"pPdgM:pPt:pEta:pPhi:pM:l1PtMc:l1PtRc:l1Eta:l1Phi:l1M:l2PtMc:l2PtRc:l2Eta:l2Phi:l2M:nPtMc:nEta:nPhi:nM:Mll:weight" );
	
	// parent pdg Mass
	// parent 4-vector
	// lepton1 Pt MC
	// lepton1 4-vector (with RECO Pt)
	// lepton2 Pt MC
	// lepton2 4-vector (with RECO Pt)
	// neutral 4-vector (if available)
	// Reco dilepton Mass 
	// weight assigned at time of generation - utility for all-in-one scaling
}


void CocktailNTupleMaker::postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd ){
	CocktailMaker::postDecay( _name, _parent, _pd );

	// Get the two lepton decay products
	TLorentzVector l1lv = _pd.getLepton1().lv;
	TLorentzVector l2lv = _pd.getLepton2().lv;
	
	// initialize the tuple data to 0
	Float_t data[21] = {0};


	// fill the ntuple data
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
	data[20] = namedWeight[ _name ];

	ntuple->Fill( data );

}

void CocktailNTupleMaker::postMake(){
	INFO( classname(), "Closing the file and NTuple" );
}