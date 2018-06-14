#include "Makers/DecayNTupleMaker.h"


string DecayNTupleMaker::ntupleSchema = "pdgM:pt:eta:phi:m:y:l1PtMc:l1PtRc:l1Eta:l1Phi:l1M:l2PtMc:l2PtRc:l2Eta:l2Phi:l2M:nPtMc:nEta:nPhi:nM:Mll:weight:pMcId:decay";
string DecayNTupleMaker::ntupleName = "ll";


void DecayNTupleMaker::initialize(){
	DecayMaker::initialize();

	// open the TFile
	// file = new TFile( config.getXString( nodePath + ".output.TFile:url" ).c_str(), "RECREATE" );
	// setup the ntuple
	book->cd();
	ntuple = new TNtuple( DecayNTupleMaker::ntupleName.c_str(), "Dilepton cocktail tuple format",
		DecayNTupleMaker::ntupleSchema.c_str() );
	
	// parent pdg Mass
	// parent 4-vector
	// lepton1 Pt MC
	// lepton1 4-vector (with RECO Pt)
	// lepton2 Pt MC
	// lepton2 4-vector (with RECO Pt)
	// neutral 4-vector (if available)
	// Reco dilepton Mass 
	// weight assigned at time of generation - utility for all-in-one scaling
	// parent MC Id 
	// decay type 2=twobody, 3=dalitz
	

	makeHistos = false;
}


void DecayNTupleMaker::postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd ){
	DecayMaker::postDecay( _name, _parent, _pd );

	// Get the two lepton decay products
	TLorentzVector l1lv = _pd.getLepton1().lv;
	TLorentzVector l2lv = _pd.getLepton2().lv;
	
	// initialize the tuple data to 0
	Float_t data[23] = {0};


	// fill the ntuple data
	data[0]  = _parent.M();		    // parent PDG mass
	data[1]  = _parent.Pt();		// parent 4 - vector with sampled mass
	data[2]  = _parent.Eta();
	data[3]  = _parent.Phi();
	data[4]  = _pd.getSampledMass();
	data[5]  = _parent.Rapidity();

	data[6]  = l1lv.Pt();
	data[7]  = rclv1.Pt();
	data[8]  = l1lv.Eta();
	data[9]  = l1lv.Phi();
	data[10]  = l1lv.M();

	data[11] = l2lv.Pt();
	data[12] = rclv2.Pt();
	data[13] = l2lv.Eta();
	data[14] = l2lv.Phi();
	data[15] = l2lv.M();

	if ( 3 == _pd.getNProducts() ){
		TLorentzVector nlv  = _pd.getNeutral().lv;
		data[16] = nlv.Pt();	
		data[17] = nlv.Eta();
		data[18] = nlv.Phi();
		data[19] = nlv.M();
	}

	data[20] = rclv.M();
	data[21] = namedWeight[ _name ];
	data[21] = _pd.getParentInfo().mcId;
	data[22] = _pd.getNProducts();

	ntuple->Fill( data );

}

void DecayNTupleMaker::postMake(){
	INFO( classname(), "Closing the file and NTuple" );
}