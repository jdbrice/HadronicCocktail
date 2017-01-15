#ifndef BLENDER_H
#define BLENDER_H

#include "TreeAnalyzer.h"
#include "CutCollection.h"
using namespace jdb;


#include <sstream>
using namespace std;

#include "TLorentzVector.h"

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


		// load the active channel info
		vector<string> pc = config.childrenOf( nodePath + ".ActiveChannels", "ActiveChannel" );
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

		ccol.init( config, nodePath + ".Cuts" );
		INFOC( "===================CUTS===================");
		ccol.report();
		INFOC( "===================CUTS===================");

	}

protected:

	virtual void preEventLoop(){
		TreeAnalyzer::preEventLoop();

		for ( auto kv : scale ){
			for ( string hn : { "dNdM", "pRapidity", "pEta", "pre_pRapidity", "pre_pEta", "l1PtRc", "l2PtRc", "pre_l1PtRc", "pre_l2PtRc", "l1Eta", "l2Eta", "pre_l1Eta", "pre_l2Eta" } ){
				book->clone( hn, hn + "_" + kv.first );
			}
			
		}
	}

	virtual void analyzeEvent(){

		
		string key = ts((int)pMcId) +"_" + ts((int)decay);
		

		if ( 0 == nameForPlcDecay.count( key ) ) return;

		string name = nameForPlcDecay[ key ];

		// count N for taking care of possible different num of each component
		Nobs[ name ] ++;

		TLorentzVector lv;
		lv.SetPtEtaPhiM( pPt, pEta, pPhi, Mll );
		pRapidity = lv.Rapidity();

		book->fill( "pre_l1PtRc_" + name, l1PtRc );
		book->fill( "pre_l2PtRc_" + name, l2PtRc );
		book->fill( "pre_l1Eta_" + name, l1Eta );
		book->fill( "pre_l2Eta_" + name, l2Eta );
		book->fill( "pre_pEta_" + name, pEta );
		book->fill( "pre_pRapidity_" + name, pRapidity );

		for ( auto kv : tvars ) {
			if ( ccol.has( kv.first ) ){
				if ( !ccol[ kv.first ]->inInclusiveRange( (*kv.second) ) ) return;
			}
		}



		// redo mom smearing

		// redo mom smearing
		

		book->fill( "dNdM_" + name, Mll );
		book->fill( "l1PtRc_" + name, l1PtRc );
		book->fill( "l2PtRc_" + name, l2PtRc );
		book->fill( "l1Eta_" + name, l1Eta );
		book->fill( "l2Eta_" + name, l2Eta );
		book->fill( "pEta_" + name, pEta );
		book->fill( "pRapidity_" + name, pRapidity );

	}

	virtual void postEventLoop(){
		TreeAnalyzer::postEventLoop();

		double Nmb = config.getDouble( nodePath + "nMinBias", 1000 );

		// scale each component and add to total
		for ( auto kv : scale ){
			string hn = "dNdM_" + kv.first;
			int _Nobs = Nobs[ kv.first ];
			if ( 0 == _Nobs ){
				ERRORC( "0 " << kv.first << " observed !!!" );
				continue;
			}

			// TODO: add scale for dy range (0.8 - -0.8) = 1.6

			INFOC( "Scale for " << quote(kv.first) << " : " << kv.second << "*" << "1.0/" << _Nobs << " = " << (kv.second * 1.0 / _Nobs) );
			book->get( hn )->Scale( kv.second * (1.0 / _Nobs) * ( Nmb ) );

			book->get( "dNdM" )->Add( book->get( hn ) );

		}

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


	map<string, Float_t*> tvars;
	map<string, double> dndy;
	map<string, double> br;
	map<string, double> scale;
	map<string, string> nameForPlcDecay;
	map<string, int> Nobs;


	CutCollection ccol;


};


#endif