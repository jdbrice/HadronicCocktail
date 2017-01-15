#ifndef CONVERT_CCBAR_H
#define CONVERT_CCBAR_H

#include "TreeAnalyzer.h"
using namespace jdb;

#include "TNtuple.h"


class ConvertCCbar : public TreeAnalyzer
{
public:
	ConvertCCbar() {}
	~ConvertCCbar() {}
	virtual void initialize(){
		TreeAnalyzer::initialize();

		tvars[ "nPt" ] = &nPt;
		tvars[ "nEta" ] = &nEta;
		tvars[ "nPhi" ] = &nPhi;
		tvars[ "nMass" ] = &nMass;
		tvars[ "nParentId" ] = &nParentId;
		tvars[ "pPt" ] = &pPt;
		tvars[ "pEta" ] = &pEta;
		tvars[ "pPhi" ] = &pPhi;
		tvars[ "pMass" ] = &pMass;
		tvars[ "pParentId" ] = &pParentId;
		tvars[ "pairPt" ] = &pairPt;
		tvars[ "pairEta" ] = &pairEta;
		tvars[ "pairPhi" ] = &pairPhi;
		tvars[ "pairMass" ] = &pairMass;
		tvars[ "pairY" ] = &pairY;


		for ( auto kv : tvars ){
			chain->SetBranchAddress( kv.first.c_str(), kv.second );
		}

		vector<string> pbrs = config.childrenOf( nodePath, "BranchingRatio" );
		for ( string p : pbrs ){
			int mcId   = config.getInt( p + ":parent" );
			double br  = config.getDouble( p + ":br" );
			double ebr = config.getDouble( p + ":ebr" );

			branchingRatio[ mcId ]      = br;
			branchingRatioError[ mcId ] = ebr;

			INFOC( "BranchingRatio [" << mcId << "] = " << br << " +/- " << ebr );
		}

		sig_cc = config.getDouble( nodePath + ".CrossSections:ccbar", 0 );
		sig_mb = config.getDouble( nodePath + ".CrossSections:minbias", 42 );



		book->cd();
		ntuple = new TNtuple( CocktailNTupleMaker::ntupleName.c_str(), "Dilepton cocktail tuple format",
		CocktailNTupleMaker::ntupleSchema.c_str() );

	}
protected:

	virtual void preEventLoop(){
		TreeAnalyzer::preEventLoop();

		Ncc = chain->GetEntries();
	}

	virtual void analyzeEvent(){

		// INFOC( "pairMass = " << pairMass );


		Float_t data[23] = {0};

		

		float br1 = branchingRatio[ abs(pParentId) ];
		float br2 = branchingRatio[ abs(nParentId) ];

		float weight = 1.0;
		weight *= (br1 * br2);
		DEBUGC( "weight = " << br1 << "*" << br2 << " = " << weight );
		weight *= (1.0 / Ncc);
		weight *= (sig_cc / sig_mb);

		

		// fill the ntuple data
		data[0]  = 0;		    // parent PDG mass
		data[1]  = pairPt;		// parent 4 - vector with sampled mass
		data[2]  = pairEta;
		data[3]  = pairPhi;
		data[4]  = pairMass;

		data[5]  = pParentId;
		data[6]  = pPt;
		data[7]  = pEta;
		data[8]  = pPhi;
		data[9]  = pMass;

		data[10] = nParentId;
		data[11] = nPt;
		data[12] = nEta;
		data[13] = nPhi;
		data[14] = nMass;

		// 4-vector neutral part in dalitz decay
		data[15] = 0;
		data[16] = 0;
		data[17] = 0;
		data[18] = 0;
		

		data[19] = pairMass;
		data[20] = weight; 	// BR! * BR2 based on mu parent
		data[21] = 4; // mcId of single c quark
		data[22] = 2;

		ntuple->Fill( data );


	}

	virtual void postEventLoop() {
		TreeAnalyzer::postEventLoop();
	}


	TNtuple *ntuple;


	Float_t nPt;
	Float_t nEta;
	Float_t nPhi;
	Float_t nMass;
	Float_t nParentId;
	Float_t pPt;
	Float_t pEta;
	Float_t pPhi;
	Float_t pMass;
	Float_t pParentId;
	Float_t pairPt;
	Float_t pairEta;
	Float_t pairPhi;
	Float_t pairMass;
	Float_t pairY;

	map<string, Float_t*> tvars;

	map<int, double> branchingRatio;
	map<int, double> branchingRatioError;

	ULong_t Ncc = 0;
	double sig_cc = 0.0;
	double sig_mb = 0.0;

};

#endif