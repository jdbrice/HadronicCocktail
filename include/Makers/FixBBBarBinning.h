#ifndef FIX_BBBAR_BINNING_H
#define FIX_BBBAR_BINNING_H

#include "HistoAnalyzer.h"

#include "FunctionLibrary.h"
#include "KinematicFilter.h"

class FixBBBarBinning : public HistoAnalyzer {
protected:

public:
	FixBBBarBinning() {}
	~FixBBBarBinning() {}

	virtual void initialize(){
		HistoAnalyzer::initialize();
		book->cd();
	}

	void prepareHistos(){

		book->makeAll( config, nodePath + ".histograms" );
		vector<string> states = { "FullAcc_", "PairCut_", "AccCut0_", "AccCut1_" };
		vector<string> histos = { "dNdM_pT" };

		for ( string s : states ){
			for ( string hn : histos ){
				book->clone( hn, s + hn );
			} // histos
		} // states
	}

	virtual void manualCopyBins( TH2 * hSource, TH2 *hTarget ){

		auto x = hSource->GetXaxis();
		auto y = hSource->GetYaxis();

		for ( int ix = 1; ix <= x->GetNbins(); ix++ ){
			for ( int iy = 1; iy <= y->GetNbins(); iy++ ){
				int globalBin = hSource->GetBin( ix, iy );
				hTarget->SetBinContent( globalBin, hSource->GetBinContent( globalBin ) );
			} // iy
		}// ix
	}

	virtual void make(){

		prepareHistos();

		TH2 * hFullAcc = get<TH2>( "hFullAcc_dNdM_pT" );
		TH2 * hPairCut = get<TH2>( "hPairCut_dNdM_pT" );
		TH2 * hAccCut0 = get<TH2>( "hAccCut0_dNdM_pT" );
		TH2 * hAccCut1 = get<TH2>( "hAccCut1_dNdM_pT" );

		manualCopyBins( hFullAcc, dynamic_cast<TH2*>(book->get( "FullAcc_dNdM_pT" )) );
		manualCopyBins( hPairCut, dynamic_cast<TH2*>(book->get( "PairCut_dNdM_pT" )) );
		manualCopyBins( hAccCut0, dynamic_cast<TH2*>(book->get( "AccCut0_dNdM_pT" )) );
		manualCopyBins( hAccCut1, dynamic_cast<TH2*>(book->get( "AccCut1_dNdM_pT" )) );
	}

protected:
};



#endif