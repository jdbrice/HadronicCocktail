#ifndef BLENDER_H
#define BLENDER_H

#include "TreeAnalyzer.h"
#include "CutCollection.h"
using namespace jdb;

//Project
#include "FunctionLibrary.h"
#include "KinematicFilter.h"

#include <sstream>
using namespace std;

#include "TLorentzVector.h"
#include "TRandom3.h"


class Blender : public TreeAnalyzer
{
public:
	virtual const char* classname( ) const { return "Blender"; }
	Blender() {}
	~Blender() {}

	virtual void initialize(){
		TreeAnalyzer::initialize();

		gErrorIgnoreLevel = kError;

		//add the ccbar tree file if present
		if ( config.exists( nodePath + ".input.ccbar:url" ) ){
			INFOC( "Adding CCbar from : " << config.getString( nodePath + ".input.ccbar:url" ) );
			chain->Add( config.getString( nodePath + ".input.ccbar:url" ).c_str() );
		}

		tvars[ "pPdgM" ]  = &pPdgM;
		tvars[ "pPt" ]    = &pPt;
		tvars[ "pEta" ]   = &pEta;
		tvars[ "pPhi" ]   = &pPhi;
		tvars[ "pM" ]     = &pM;
		tvars[ "l1PtMc" ] = &l1PtMc;
		tvars[ "l1PtRc" ] = &l1PtRc;
		tvars[ "l1Eta" ]  = &l1Eta;
		tvars[ "l1Phi" ]  = &l1Phi;
		tvars[ "l1M" ]    = &l1M;
		tvars[ "l2PtMc" ] = &l2PtMc;
		tvars[ "l2PtRc" ] = &l2PtRc;
		tvars[ "l2Eta" ]  = &l2Eta;
		tvars[ "l2Phi" ]  = &l2Phi;
		tvars[ "l2M" ]    = &l2M;
		tvars[ "nPtMc" ]  = &nPtMc;
		tvars[ "nEta" ]   = &nEta;
		tvars[ "nPhi" ]   = &nPhi;
		tvars[ "nM" ]     = &nM;
		tvars[ "Mll" ]    = &Mll;
		tvars[ "weight" ] = &weight;
		tvars[ "pMcId" ]  = &pMcId;
		tvars[ "decay" ]  = &decay;

		for ( auto kv : tvars ){
			chain->SetBranchAddress( kv.first.c_str(), kv.second );
			// ccol.setDefault( kv.first, -1e8, 1e8 );
		}

		// my own, non-tree variabls
		tvars[ "pRapidity" ]     = &pRapidity;
		tvars[ "rMll" ]          = &rMll;
		tvars[ "tPt" ]           = &rPt;


		// load the active channel info
		INFOC( "======================ACTIVE CHANNELS=====================" );
		vector<string> pc = config.childrenOf( "ActiveChannels", "ActiveChannel" );
		stringstream rp;
		for ( string p : pc ){
			string n               = config.getXString( p + ":name" );
			double _dndy           = config.getDouble( p + ":dndy" );
			double _br             = config.getDouble( p + ":br" );
			int _pMcId             = config.getInt( p + ":pMcId" );
			int _decay             = config.getInt( p + ":decay" );

			string key             = ts(_pMcId) + "_" + ts(_decay);

			dndy[ n ]              = _dndy;
			br[ n ]                = _br;
			scale[ n ]             = _dndy * _br;
			nameForPlcDecay[ key ] = n;
			Nobs[ n ]              = 0;

			rp << "\tActiveChannel[" <<n<<"("<<key<<")] dN/dy=" << dndy[n] << ", BR=" << br[n] << ", scale=" << scale[ n ] << endl;
		}
		INFOC( "\n" << rp.str() );
		INFOC( "======================ACTIVE CHANNELS=====================" );

		ccol.init( config, nodePath + ".Cuts" );
		INFOC( "===================CUTS===================");
		ccol.report();
		INFOC( "===================CUTS===================");

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

		INFO( classname(), "Initializing PARENT kinematic filter");
		parentFilter.load( config, nodePath + ".KinematicFilters.Parent" );
		INFO( classname(), "Initializing DAUGHTER kinematic filter");
		daughterFilter.load( config, nodePath + ".KinematicFilters.Daughter" );

		grnd.SetSeed(0);


		momSmearing = config.getBool( nodePath + ".MomentumSmearing", true );
		INFOC( "==========MOMETUM SMEARING=================" );
		INFOC( "" << bts( momSmearing ) );
		INFOC( "==========MOMETUM SMEARING=================" );

		histoLib.loadAll( config, nodePath+".HistogramLibrary" );
		eff_mup = histoLib.get( "eff_mup" );
		eff_mum = histoLib.get( "eff_mum" );
		eff_pid = histoLib.get( "eff_pid" );



		applyEfficiency = config.getBool( nodePath + ".Efficiency:apply", false );
		INFOC( "=================Efficiency==================" );
		INFOC( "" << bts( applyEfficiency) );
		INFOC( "mu+  : " << eff_mup );
		INFOC( "mu-  : " << eff_mum );
		INFOC( "pid  : " << eff_pid );
		INFOC( "trig : " );
		INFOC( "=================Efficiency==================" );


	}

protected:

	virtual void preEventLoop(){
		TreeAnalyzer::preEventLoop();


		vector<string> states = { "FullAcc_", "RapCut_", "AccCut_" };
		vector<string> histos = { "dNdM", "dNdM_pT", "PtRc", "PtMc", "Eta", "rapidity", "Eta_vs_l2Eta" };
		vector<string> ls     = { "l1", "l2", "w" };

		for ( string s : states ){
			for ( string hn : histos ){
				book->clone( hn, s + hn );
				for ( string l : ls ){
					if ( "w" != l && ("dNdM" == hn || "dNdM_pT" == hn) ) continue;
					// if ( "w" == l && "dNdM" != hn && "dNdM_pT" == hn ) continue;
					book->clone( hn, s + l + hn );
				}
			}
		}

		
		// for ( auto kv : scale ){
		// 	for ( string hn : { "FullAcc_dNdM_pT", "mc_dNdM_pT", "dNdM_pT", "dNdM", "pRapidity", "pEta", "pre_pRapidity", "pre_pEta", "l1PtRc", "l2PtRc", "l1PtMc", "l2PtMc", "pre_l1PtRc", "pre_l2PtRc", "pre_l1PtMc", "pre_l2PtMc", "l1Eta", "l2Eta", "pre_l1Eta", "pre_l2Eta" } ){
		// 		book->clone( hn, hn + "_" + kv.first );

		// 		book->get( hn )->SetDirectory( nullptr );
		// 	}
		// }
	}



	virtual void fillState( 	string _s, 
								TLorentzVector &_lvMc, TLorentzVector &_lvRc, 
								TLorentzVector &_lvMc1, TLorentzVector &_lvRc1, 
								TLorentzVector &_lvMc2, TLorentzVector &_lvRc2,
								double _w = 1.0 ){

		book->fill( _s + "dNdM", _lvRc.M() );
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


	virtual void analyzeEvent(){
		if( nullptr == momShape || nullptr == momResolution ) return;

		string key = ts((int)pMcId) +"_" + ts((int)decay);
		if ( 0 == nameForPlcDecay.count( key ) ) return;
		string name = nameForPlcDecay[ key ];

		double w = 1.0;
		if ( "ccbar_mumu" == name )
			w = weight;		// keep the pre calculated BR scaling in ccbar, otherwise ignore

		// only needed for X->mumu
		if ( pM < l1M + l2M ) return;
		
		TLorentzVector mclv, mclv1, mclv2;
		mclv.SetPtEtaPhiM( pPt, pEta, pPhi, pM );
		mclv1.SetPtEtaPhiM( l1PtMc, l1Eta, l1Phi, l1M );
		mclv2.SetPtEtaPhiM( l2PtMc, l2Eta, l2Phi, l2M );

		// =============================== MOMENTUM SMEARING ===============================
		TLorentzVector rclv, rclv1, rclv2;
		double ptRes = momResolution->Eval( mclv1.Pt() );// * 100.0;
		double rndCrystalBall = grnd.Gaus( 0, 1.0 );//momShape->GetRandom();
		if ( false == momSmearing ) rndCrystalBall = 0.0;
		
		rclv1.SetPtEtaPhiM(  mclv1.Pt() * (1 + rndCrystalBall * ptRes  ) , mclv1.Eta(), mclv1.Phi(), mclv1.M() );
		
		ptRes = momResolution->Eval( mclv2.Pt() );// * 100.0;
		rndCrystalBall = grnd.Gaus( 0, 1.0 );//momShape->GetRandom();
		if ( false == momSmearing ) rndCrystalBall = 0.0;

		rclv2.SetPtEtaPhiM( mclv2.Pt() * (1 + rndCrystalBall * ptRes  ) , mclv2.Eta(), mclv2.Phi(), mclv2.M() );
		rclv = rclv1 + rclv2;
		// =============================== MOMENTUM SMEARING ===============================
		


		// <=============================== EFFICIENCY ===============================>
		if ( applyEfficiency ){
			int c = 1;
			if ( grnd.Rndm( ) > 0.5  ) c = -1;

			double ew1 = efficiencyWeight( mclv1, c );
			double ew2 = efficiencyWeight( mclv2, -1 * c );

			double pidw1 = pidEfficiencyWeight( mclv1 );
			double pidw2 = pidEfficiencyWeight( mclv2 );
			w *= (ew1*ew2) * ( pidw1*pidw2 );
		}
		// </=============================== EFFICIENCY ===============================>

		fillState( "FullAcc_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

		if ( !ccol.has( "rapidity" ) || ccol["rapidity"]->inInclusiveRange( rclv.Rapidity() ) ){
			fillState( "RapCut_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

			// check the kinematic filters
			if ( daughterFilter.pass( rclv1, rclv2 ) && parentFilter.pass( rclv ) ){
				fillState( "AccCut_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

			} // PASS kinematic filters
		} // PASS rapidity cut on parent
	} // analyzeEvent








	// virtual void analyzeEvent(){
	// 	if( nullptr == momShape || nullptr == momResolution ) return;
		
		

	// 	double fullWeight = 1.0;
	// 	if ( "ccbar_mumu" == name )
	// 		fullWeight = weight;


	// 	// only needed for X->mumu
	// 	if ( pM < l1M + l2M ) return;
	// 	TLorentzVector lv, lv1, lv2;
	// 	lv.SetPtEtaPhiM( pPt, pEta, pPhi, Mll );
	// 	pRapidity = lv.Rapidity();

	// 	book->fill( "pre_pEta_" + name, pEta );
	// 	book->fill( "pre_pRapidity_" + name, pRapidity );
	// 	book->fill( "pre_l1PtRc_" + name, l1PtMc );
	// 	book->fill( "pre_l2PtRc_" + name, l2PtMc );
	// 	book->fill( "pre_l1PtMc_" + name, l1PtMc );
	// 	book->fill( "pre_l2PtMc_" + name, l2PtMc );
	// 	book->fill( "pre_l1Eta_" + name, l1Eta );
	// 	book->fill( "pre_l2Eta_" + name, l2Eta );
	// 	book->fill( "FullAcc_dNdM_pT_" + name, pM, pPt, fullWeight );

	// 	if ( parentFilter.fail( lv ) ) return;	// must not have momentum cuts!
		
	// 	// count N for taking care of possible different num of each component
	// 	Nobs[ name ] ++;

		
		
		// redo mom smearing

		
	// 	lv1.SetPtEtaPhiM( l1PtMc, l1Eta, l1Phi, l1M );
	// 	lv2.SetPtEtaPhiM( l2PtMc, l2Eta, l2Phi, l2M );

		

		


	// 	// redo mom smearing
	// 	TLorentzVector rlv1, rlv2;
	// 	double ptRes = momResolution->Eval( l1PtMc );// * 100.0;
	// 	double rndCrystalBall = grnd.Gaus( 0, 1.0 );//momShape->GetRandom();
	// 	if ( false == momSmearing ) rndCrystalBall = 0.0;
	// 	rlv1.SetPtEtaPhiM( 
	// 		l1PtMc * (1 + rndCrystalBall * ptRes  ) ,
	// 		l1Eta,
	// 		l1Phi,
	// 		l1M );
		
	// 	ptRes = momResolution->Eval( l2PtMc );// * 100.0;
	// 	rndCrystalBall = grnd.Gaus( 0, 1.0 );//momShape->GetRandom();
	// 	if ( false == momSmearing ) rndCrystalBall = 0.0;
	// 	rlv2.SetPtEtaPhiM( 
	// 		l2PtMc * (1 + rndCrystalBall * ptRes  ) ,
	// 		l2Eta,
	// 		l2Phi,
	// 		l2M );
		

	// 	TLorentzVector rlv = rlv1 + rlv2;
	// 	rMll = rlv.M();
	// 	rPt  = rlv.Pt();
	// 	// redo mom smearing
		

	// 	// Cuts, specifically for pair pT
	// 	for ( auto kv : tvars ) {
	// 		if ( ccol.has( kv.first ) ){
	// 			if ( !ccol[ kv.first ]->inInclusiveRange( (*kv.second) ) ) return;
	// 		}
	// 	}


	// 	// Apply kinematic filter
	// 	if ( daughterFilter.fail( rlv1, rlv2 ) ) return;

		
	// 	// if ( rlv.Pt() < 2.5 ) return;

		

	// 	if ( applyEfficiency ){
	// 		// INFOC( "l1PtRc = " << l1PtRc );
	// 		// INFOC( "l2PtRc = " << l2PtRc );
	// 		int c = 1;
	// 		if ( grnd.Rndm( ) > 0.5  ) c = -1;

	// 		double ew1 = efficiencyWeight( lv1, c );
	// 		double ew2 = efficiencyWeight( lv2, -1 * c );
	// 		fullWeight *= (ew1*ew2);
	// 	}

		
	// 	book->fill( "dNdM_" + name, rlv.M(), fullWeight );
	// 	book->fill( "dNdM_pT_" + name, rlv.M(), rlv.Pt(), fullWeight );
	// 	book->fill( "mc_dNdM_pT_" + name, pM, lv.Pt(), fullWeight );
		
	// 	book->fill( "l1PtRc_" + name, rlv1.Pt() );
	// 	book->fill( "l2PtRc_" + name, rlv2.Pt() );
	// 	book->fill( "l1PtMc_" + name, l1PtMc );
	// 	book->fill( "l2PtMc_" + name, l2PtMc );
	// 	book->fill( "l1Eta_" + name, l1Eta );
	// 	book->fill( "l2Eta_" + name, l2Eta );
	// 	book->fill( "pEta_" + name, pEta );
	// 	book->fill( "pRapidity_" + name, pRapidity );

	// }

	virtual void postEventLoop(){
		TreeAnalyzer::postEventLoop();

		// double Nmb = config.getDouble( nodePath + "nMinBias", 1 );

		// // scale each component and add to total
		// for ( auto kv : scale ){
		// 	string hn = "dNdM_" + kv.first;

		// 	if ( "ccbar_mumu" == kv.first ){
				
		// 		// we want the histograms without weight


		// 		book->get( "dNdM" )->Add( book->get( hn ) );
		// 		continue;
		// 	}

		// 	int _Nobs = Nobs[ kv.first ];
		// 	if ( 0 == _Nobs ){
		// 		ERRORC( "0 " << kv.first << " observed !!!" );
		// 		continue;
		// 	}

		// 	// dN/dy * BR * (1.0 / N)
		// 	double sf = kv.second * (1.0 / _Nobs) * ( 1.0 / ((float)NJobs) ) ;
		// 	INFOC( "Scale for " << quote(kv.first) << " : " << kv.second << "*" << "1.0/" << _Nobs << " = " << (kv.second * 1.0 / _Nobs) );
		// 	book->get( hn )->Scale( sf );
		// 	book->get( "dNdM_pT_" + kv.first )->Scale( sf );

		// 	book->get( "dNdM" )->Add( book->get( hn ) );
		// }
	}

	double efficiencyWeight( TLorentzVector &_lv, int _charge ){

		shared_ptr<TH1> table = nullptr;
		if ( -1 == _charge ) table = eff_mum;
		if (  1 == _charge ) table = eff_mup;

		if ( nullptr == table ) {
			ERRORC( "INVALID EFF TABLE! charge=" << _charge );
			return 1.0;
		}

		// 2d pT vs. eta
		TAxis *x = table->GetXaxis();
		TAxis *y = table->GetYaxis();

		int bx = x->FindBin(_lv.Eta());
		int by = y->FindBin(_lv.Pt());

		double w = table->GetBinContent( bx, by );
		// INFOC( "Eff( pT=" << _lv.Pt() <<", Eta=" << _lv.Eta() <<") = " << w );


		return w;
	}


	double pidEfficiencyWeight( TLorentzVector &_lv ){
		if ( nullptr == eff_pid ) {
			ERRORC( "INVALID PID EFFICIENCY TABLE!" );
			return 1.0;
		}
		// 1d vs. pT
		TAxis *x = eff_pid->GetXaxis();
		int bx = x->FindBin(_lv.Pt());
		double w = eff_pid->GetBinContent( bx );
		return w;
	}


	// data members of the tntuple
	Float_t pPdgM;
	Float_t pPt;
	Float_t pEta;
	Float_t pPhi;
	Float_t pM;
	Float_t l1PtMc;
	Float_t l1PtRc;
	Float_t l1Eta;
	Float_t l1Phi;
	Float_t l1M;
	Float_t l2PtMc;
	Float_t l2PtRc;
	Float_t l2Eta;
	Float_t l2Phi;
	Float_t l2M;
	Float_t nPtMc;
	Float_t nEta;
	Float_t nPhi;
	Float_t nM;
	Float_t Mll;
	Float_t weight;
	Float_t pMcId;
	Float_t decay;

	// My own dynamically calculated vars
	Float_t pRapidity;
	Float_t rMll;
	Float_t rPt;


	map<string, Float_t*> tvars;
	map<string, double> dndy;
	map<string, double> br;
	map<string, double> scale;
	map<string, string> nameForPlcDecay;
	map<string, int> Nobs;


	CutCollection ccol;

	// for momentum smearing
	FunctionLibrary funLib;
	shared_ptr<TF1>				  momResolution;
	shared_ptr<TF1>				  momShape;

	KinematicFilter 			  parentFilter;
	KinematicFilter 			  daughterFilter;

	TRandom3 grnd;
	bool momSmearing;

	bool applyEfficiency = false;
	HistogramLibrary histoLib;
	shared_ptr<TH1> eff_mup = nullptr;
	shared_ptr<TH1> eff_mum = nullptr;
	shared_ptr<TH1> eff_pid = nullptr;


};


#endif