#ifndef HISTOGRAM_MAKER_H
#define HISTOGRAM_MAKER_H

#include "TreeAnalyzer.h"

#include "FunctionLibrary.h"
#include "KinematicFilter.h"

class HistogramMaker : public TreeAnalyzer {
protected:
	map<string, Float_t> tvars;

	map<int, double> branchingRatio;
	map<int, double> branchingRatioError;

	ULong_t Ncc = 0;
	double sig_cc = 0.0;
	double sig_mb = 0.0;

	FunctionLibrary funLib;

	shared_ptr<TF1>				momResolution;
	shared_ptr<TF1>				momShape;

	bool momSmearing = false;
	bool makeTF1 = false;

	KinematicFilter 			parentFilter;
	KinematicFilter 			daughterFilter;
public:
	HistogramMaker() {}
	~HistogramMaker() {}

	virtual void initialize(){
		TreeAnalyzer::initialize();

		tvars[ "nPt" ];
		tvars[ "nEta" ];
		tvars[ "nPhi" ];
		tvars[ "nMass" ];
		tvars[ "nParentId" ];
		tvars[ "pPt" ];
		tvars[ "pEta" ];
		tvars[ "pPhi" ];
		tvars[ "pMass" ];
		tvars[ "pParentId" ];
		tvars[ "pairPt" ];
		tvars[ "pairEta" ];
		tvars[ "pairPhi" ];
		tvars[ "pairMass" ];
		tvars[ "pairY" ];

		map<string, Float_t>::iterator it;
		for (it = tvars.begin(); it != tvars.end(); it++ ){
			chain->SetBranchAddress( it->first.c_str(), &it->second );
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


		int seed = config.getInt( "SEED", 0 );
	
		gRandom = new TRandom3();
		INFO( classname(), "Initialize RANDOM SEED = " << seed );
		gRandom->SetSeed( seed );


		funLib.loadAll( config, nodePath + ".FunctionLibrary" );
		momResolution = funLib.get( "pT_Resolution" );
		momShape      = funLib.get( "pT_Shape" );
		if ( nullptr == momShape ){
			ERROR( classname(), "MUST provide a function for the Momentum Smearing Shape with name " << quote( "pT_Shape" ) << ". Using default CrystalBall shape with made-up parameters" );
			momShape = shared_ptr<TF1>( new TF1( "momShape", CrystalBall2, -1, 1, 7 ) );
			momShape->SetParameters(1., -1e-3, 0.01, 1.29, 1.75, 2.92, 1.84);	
		}
		if ( nullptr == momResolution ){
			ERRORC( "Must provide a function with the name " << quote( "pT_Resolution" ) );
		}

		momSmearing = config.getBool( nodePath + ".MomentumSmearing", true );
		INFOC( "==========MOMETUM SMEARING=================" );
		INFOC( "" << bts( momSmearing ) );
		INFOC( "==========MOMETUM SMEARING=================" );

		if ( makeTF1 ){
			momShape->Write();
			momResolution->Write();
		}


		INFO( classname(), "Initializing PARENT kinematic filter");
		parentFilter.load( config, nodePath + ".KinematicFilters.Parent" );
		INFO( classname(), "Initializing DAUGHTER kinematic filter");
		daughterFilter.load( config, nodePath + ".KinematicFilters.Daughter" );

	}

	virtual void preEventLoop(){
		TreeAnalyzer::preEventLoop();

		prepareHistos();
	}

	void prepareHistos(){
		vector<string> states = { "FullAcc_", "RapCut_", "AccCut_" };
		vector<string> histos = { "dNdM", "dNdM_pT", "dNdM_pT_eff", "PtRc", "PtMc", "Eta", "rapidity", "Eta_vs_l2Eta" };
		vector<string> ls     = { "l1", "l2", "w" };

		for ( string s : states ){
			for ( string hn : histos ){
				book->clone( hn, s + hn );
				for ( string l : ls ){
					if ( "w" != l && ("dNdM" == hn || "dNdM_pT" == hn) ) continue;
					book->clone( hn, s + l + hn );
				}// ls
			} // histos
		} // states
	}

	virtual void analyzeEvent(){
		TLorentzVector mclv;
		mclv.SetPtEtaPhiM( tvars["pairPt"], tvars["pairEta"], tvars["pairPhi"], tvars["pairMass"] );
		TLorentzVector mclv1, mclv2;
		mclv1.SetPtEtaPhiM( tvars["nPt"], tvars["nEta"], tvars["nPhi"], 0.105 );
		mclv2.SetPtEtaPhiM( tvars["pPt"], tvars["pEta"], tvars["pPhi"], 0.105 );

		// =============================== MOMENTUM SMEARING ===============================
		TLorentzVector rclv, rclv1, rclv2;
		double ptRes = momResolution->Eval( mclv1.Pt() );
		double rndCrystalBall = 0.0;
		if ( true == momSmearing ) rndCrystalBall = gRandom->Gaus( 0, 1.0 );
		
		rclv1.SetPtEtaPhiM(  mclv1.Pt() * (1 + rndCrystalBall * ptRes  ), mclv1.Eta(), mclv1.Phi(), mclv1.M() );
		
		ptRes = momResolution->Eval( mclv2.Pt() );
		rndCrystalBall = 0.0;
		if ( true == momSmearing ) rndCrystalBall = gRandom->Gaus( 0, 1.0 );

		rclv2.SetPtEtaPhiM( mclv2.Pt() * (1 + rndCrystalBall * ptRes  ) , mclv2.Eta(), mclv2.Phi(), mclv2.M() );
		rclv = rclv1 + rclv2;
		// =============================== MOMENTUM SMEARING ===============================

		double w = 1.0; // could support weigthing, no use now
		fillState( "FullAcc_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

		if ( parentFilter.pass( rclv ) ){
			fillState( "RapCut_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

			// check the kinematic filters
			if ( daughterFilter.pass( rclv1, rclv2 ) ){
				fillState( "AccCut_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

			} // PASS kinematic filters
		} // PASS rapidity cut on parent
	}

	virtual void postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd ){
		
	}

	void fillState( string _s, 
										TLorentzVector &_lvMc, TLorentzVector &_lvRc, 
										TLorentzVector &_lvMc1, TLorentzVector &_lvRc1, 
										TLorentzVector &_lvMc2, TLorentzVector &_lvRc2,
										double _w ){

		book->fill(_s + "dNdM", _lvRc.M() );
		book->fill( _s + "dNdM_pT", _lvRc.M(), _lvRc.Pt() );
		book->fill( _s + "wdNdM", _lvRc.M(), _w );
		book->fill( _s + "wdNdM_pT", _lvRc.M(), _lvRc.Pt(), _w );
		book->fill( _s + "PtRc", _lvRc.Pt() );
		book->fill( _s + "PtMc", _lvMc.Pt() );
		book->fill( _s + "l1PtRc", _lvRc1.Pt() );
		book->fill( _s + "l2PtRc", _lvRc2.Pt() );
		book->fill( _s + "l1PtMc", _lvMc1.Pt() );
		book->fill( _s + "l2PtMc", _lvMc2.Pt() );

		book->fill( _s + "Eta", _lvRc.Eta() );
		book->fill( _s + "Eta_vs_l2Eta", _lvRc.Eta(), _lvRc2.Eta() );
		book->fill( _s + "l1Eta", _lvRc1.Eta() );
		book->fill( _s + "l1Eta_vs_l2Eta", _lvRc1.Eta(), _lvRc2.Eta() );
		book->fill( _s + "l2Eta", _lvRc2.Eta() );
		book->fill( _s + "rapidity", _lvRc.Rapidity() );
		book->fill( _s + "l1rapidity", _lvRc1.Rapidity() );
		book->fill( _s + "l2rapidity", _lvRc2.Rapidity() );

	}

protected:
};



#endif