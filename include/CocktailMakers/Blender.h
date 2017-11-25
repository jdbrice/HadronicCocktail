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



		applyEfficiency = config.getBool( nodePath + ".Efficiency:apply", false );
		INFOC( "=================Efficiency==================" );
		INFOC( "" << bts( applyEfficiency) );
		INFOC( "=================Efficiency==================" );


	}

protected:

	virtual void preEventLoop(){
		TreeAnalyzer::preEventLoop();

		for ( auto kv : scale ){
			for ( string hn : { "mc_dNdM_pT", "dNdM_pT", "dNdM", "pRapidity", "pEta", "pre_pRapidity", "pre_pEta", "l1PtRc", "l2PtRc", "pre_l1PtRc", "pre_l2PtRc", "l1Eta", "l2Eta", "pre_l1Eta", "pre_l2Eta" } ){
				book->clone( hn, hn + "_" + kv.first );

				book->get( hn )->SetDirectory( nullptr );
			}
			
		}
	}

	virtual void analyzeEvent(){
		if( nullptr == momShape || nullptr == momResolution ) return;
		
		string key = ts((int)pMcId) +"_" + ts((int)decay);
		

		if ( 0 == nameForPlcDecay.count( key ) ) return;

		string name = nameForPlcDecay[ key ];

		// only needed for X->mumu
		if ( pM < l1M + l2M ) return;
		TLorentzVector lv, lv1, lv2;
		lv.SetPtEtaPhiM( pPt, pEta, pPhi, Mll );
		if ( parentFilter.fail( lv ) ) return;	// must not have momentum cuts!
		
		// count N for taking care of possible different num of each component
		Nobs[ name ] ++;

		
		
		pRapidity = lv.Rapidity();

		
		lv1.SetPtEtaPhiM( l1PtMc, l1Eta, l1Phi, l1M );
		lv2.SetPtEtaPhiM( l2PtMc, l2Eta, l2Phi, l2M );

		
		book->fill( "pre_l1PtRc_" + name, l1PtRc );
		book->fill( "pre_l2PtRc_" + name, l2PtRc );
		book->fill( "pre_l1Eta_" + name, l1Eta );
		book->fill( "pre_l2Eta_" + name, l2Eta );
		book->fill( "pre_pEta_" + name, pEta );
		book->fill( "pre_pRapidity_" + name, pRapidity );



		

		
		// redo mom smearing
		TLorentzVector rlv1, rlv2;
		double ptRes = momResolution->Eval( l1PtMc );// * 100.0;
		double rndCrystalBall = grnd.Gaus( 0, 1.0 );//momShape->GetRandom();
		if ( false == momSmearing ) rndCrystalBall = 0.0;
		rlv1.SetPtEtaPhiM( 
			l1PtMc * (1 + rndCrystalBall * ptRes  ) ,
			l1Eta,
			l1Phi,
			l1M );
		
		ptRes = momResolution->Eval( l2PtMc );// * 100.0;
		rndCrystalBall = grnd.Gaus( 0, 1.0 );//momShape->GetRandom();
		if ( false == momSmearing ) rndCrystalBall = 0.0;
		rlv2.SetPtEtaPhiM( 
			l2PtMc * (1 + rndCrystalBall * ptRes  ) ,
			l2Eta,
			l2Phi,
			l2M );
		

		TLorentzVector rlv = rlv1 + rlv2;
		rMll = rlv.M();
		// redo mom smearing
		

		// Cuts, specifically for pair pT
		for ( auto kv : tvars ) {
			if ( ccol.has( kv.first ) ){
				if ( !ccol[ kv.first ]->inInclusiveRange( (*kv.second) ) ) return;
			}
		}


		// Apply kinematic filter
		if ( daughterFilter.fail( rlv1, rlv2 ) ) return;

		
		// if ( rlv.Pt() < 2.5 ) return;

		double fullWeight = 1.0;
		if ( "ccbar_mumu" == name )
			fullWeight = weight;

		if ( applyEfficiency ){
			// INFOC( "l1PtRc = " << l1PtRc );
			// INFOC( "l2PtRc = " << l2PtRc );
			int c = 1;
			if ( grnd.Rndm( ) > 0.5  ) c = -1;

			double ew1 = efficiencyWeight( lv1, c );
			double ew2 = efficiencyWeight( lv2, -1 * c );
			fullWeight *= (ew1*ew2);
		}

		
		book->fill( "dNdM_" + name, rlv.M(), fullWeight );
		book->fill( "dNdM_pT_" + name, rlv.M(), rlv.Pt(), fullWeight );
		book->fill( "mc_dNdM_pT_" + name, pM, lv.Pt(), fullWeight );
		
		book->fill( "l1PtRc_" + name, l1PtRc );
		book->fill( "l2PtRc_" + name, l2PtRc );
		book->fill( "l1Eta_" + name, l1Eta );
		book->fill( "l2Eta_" + name, l2Eta );
		book->fill( "pEta_" + name, pEta );
		book->fill( "pRapidity_" + name, pRapidity );

	}

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


};


#endif