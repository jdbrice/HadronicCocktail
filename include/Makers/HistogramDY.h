#ifndef HISTOGRAM_DY_H
#define HISTOGRAM_DY_H

#include "TreeAnalyzer.h"

#include "FunctionLibrary.h"
#include "KinematicFilter.h"

class HistogramDY : public TreeAnalyzer {
protected:
	map<string, Double_t> tvars;

	FunctionLibrary funLib;

	shared_ptr<TF1>				momResolution;
	shared_ptr<TF1>				momShape;

	bool momSmearing = false;
	bool makeTF1 = false;

	KinematicFilter 			parentFilter;
	KinematicFilter 			daughterFilterA;
	KinematicFilter 			daughterFilterB;

public:
	HistogramDY() {}
	~HistogramDY() {}

	unsigned long long int get_seed(){
		unsigned long long int random_value = 0; //Declare value to store data into
		size_t size = sizeof(random_value); //Declare size of data
		ifstream urandom("/dev/urandom", ios::in|ios::binary); //Open stream
		if(urandom) //Check if stream is open
		{
			urandom.read(reinterpret_cast<char*>(&random_value), size); //Read from urandom
			if(urandom) {
				return random_value;
			}
			else { //Read failed
				return 0;
			}
			urandom.close(); //close stream
		} else { //Open failed
			std::cerr << "Failed to open /dev/urandom" << std::endl;
		}
		return 0;
	}

	bool passDaughterFilters( TLorentzVector &rclv1, TLorentzVector &rclv2, string namedCut ){
		if ( daughterFilterA.pass( rclv1, namedCut ) && daughterFilterB.pass( rclv2, namedCut )  )
			return true;
		if ( daughterFilterB.pass( rclv1, namedCut ) && daughterFilterA.pass( rclv2, namedCut )  )
			return true;
		return false;
	}

	bool passDaughterFilters( TLorentzVector &rclv1, TLorentzVector &rclv2 ){
		if ( daughterFilterA.pass( rclv1 ) && daughterFilterB.pass( rclv2 )  )
			return true;
		if ( daughterFilterB.pass( rclv1 ) && daughterFilterA.pass( rclv2 )  )
			return true;
		return false;
	}

	virtual void initialize(){
		TreeAnalyzer::initialize();

		tvars[ "ePosPt" ];
		tvars[ "ePosEta" ];
		tvars[ "ePosPhi" ];

		tvars[ "eNegPt" ];
		tvars[ "eNegEta" ];
		tvars[ "eNegPhi" ];

		tvars[ "Z0Pt" ];
		tvars[ "Z0Eta" ];
		tvars[ "Z0Phi" ];
		tvars[ "Z0Y" ];

		tvars[ "eePt" ];
		tvars[ "eeEta" ];
		tvars[ "eePhi" ];
		tvars[ "eeRapidity" ];
		tvars[ "eeM" ];

		// despite the names it really is mu+ and mu-
		// i didnt bother changing names from Shuai's code for compatibility


		map<string, Double_t>::iterator it;
		for (it = tvars.begin(); it != tvars.end(); it++ ){
			chain->SetBranchAddress( it->first.c_str(), &it->second );
		}

		book->cd();

		unsigned long long seed = get_seed();
	
		if ( 0 == seed){
			cout << "ERROR getting seed!" << endl;
			assert(0);
		}

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
		// load two possibly different filters
		if ( config.exists( nodePath + ".KinematicFilters.Daughter[1]" ) ){
			daughterFilterA.load( config, nodePath + ".KinematicFilters.Daughter[0]" );
			daughterFilterB.load( config, nodePath + ".KinematicFilters.Daughter[1]" );
		} else { // assume only one, load it into both
			daughterFilterA.load( config, nodePath + ".KinematicFilters.Daughter" );
			daughterFilterB.load( config, nodePath + ".KinematicFilters.Daughter" );
		}

	}

	virtual void preEventLoop(){
		TreeAnalyzer::preEventLoop();

		prepareHistos();
	}

	void prepareHistos(){
		vector<string> states = { "FullAcc_", "PairCut_", "AccCut0_", "AccCut1_" };
		vector<string> histos = { "dNdM", "dNdM_pT", "dNdM_pT_eff", "PtRc", "PtMc", "Eta", "YMc", "YRc", "Eta_vs_l2Eta" };
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

		TRACEC( "Building TLorentzVectors" );
		TLorentzVector mclv;
		mclv.SetPtEtaPhiM( tvars["eePt"], tvars["eeEta"], tvars["eePhi"], tvars["eeM"] );
		TLorentzVector mclv1, mclv2;
		mclv1.SetPtEtaPhiM( tvars["eNegPt"], tvars["eNegEta"], tvars["eNegPhi"], 0.105 );
		mclv2.SetPtEtaPhiM( tvars["ePosPt"], tvars["ePosEta"], tvars["ePosPhi"], 0.105 );

		TRACEC( "Momentum Smearing" );
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
		

		TRACEC( "Weights" );
		double w = 1.0;
		
		TRACEC( "Filling States w=" <<  w );
		fillState( "FullAcc_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

		if ( parentFilter.pass( rclv ) ){
			// printf("rclv[pt=%0.2f, eta=%0.2f, phi=%0.2f, y=%0.2f, M=%0.2f]\n", rclv.Pt(), rclv.PseudoRapidity(), rclv.Phi(), rclv.Rapidity(), rclv.M() );
			fillState( "PairCut_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

			// check the kinematic filters
			if ( passDaughterFilters( rclv1, rclv2, "eta" ) ){
				fillState( "AccCut0_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );
			} // PASS kinematic filters

			if ( passDaughterFilters( rclv1, rclv2 ) ){
				fillState( "AccCut1_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );
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

		double wpt = 1.0;
		book->fill(_s + "dNdM", _lvRc.M(), wpt );
		book->fill( _s + "dNdM_pT", _lvRc.M(), _lvRc.Pt(), wpt );
		
		book->fill( _s + "wdNdM", _lvRc.M(), _w * wpt );
		book->fill( _s + "wdNdM_pT", _lvRc.M(), _lvRc.Pt(), _w * wpt );

		book->fill( _s + "PtRc", _lvRc.Pt(), wpt );
		book->fill( _s + "PtMc", _lvMc.Pt(), wpt );
		book->fill( _s + "l1PtRc", _lvRc1.Pt(), wpt );
		book->fill( _s + "l2PtRc", _lvRc2.Pt(), wpt );
		book->fill( _s + "l1PtMc", _lvMc1.Pt(), wpt );
		book->fill( _s + "l2PtMc", _lvMc2.Pt(), wpt );

		book->fill( _s + "Eta", _lvRc.Eta(), wpt );
		book->fill( _s + "Eta_vs_l2Eta", _lvRc.Eta(), _lvRc2.Eta(), wpt );
		book->fill( _s + "l1Eta", _lvRc1.Eta(), wpt );
		book->fill( _s + "l1Eta_vs_l2Eta", _lvRc1.Eta(), _lvRc2.Eta(), wpt );
		book->fill( _s + "l2Eta", _lvRc2.Eta(), wpt );
		
		book->fill( _s + "YMc", _lvMc.Rapidity(), wpt );
		book->fill( _s + "YRc", _lvRc.Rapidity(), wpt );

		book->fill( _s + "l1YRc", _lvRc1.Rapidity(), wpt );
		book->fill( _s + "l2YRc", _lvRc2.Rapidity(), wpt );

		book->fill( _s + "l1YMc", _lvMc1.Rapidity(), wpt );
		book->fill( _s + "l2YMc", _lvMc2.Rapidity(), wpt );

	}

protected:
};



#endif