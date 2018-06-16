#ifndef SCALER_H
#define SCALER_H

#include "HistoAnalyzer.h"

class Scaler : public HistoAnalyzer
{
protected:
	float minPt = 0.0;

	TH1 * hsum = nullptr;

public:
	virtual const char* classname() const { return "Scaler"; }
	Scaler() {}
	~Scaler() {}


	virtual void initialize(){
		HistoAnalyzer::initialize();
		INFOC( "" );

		minPt = config.get<float>( nodePath + ".minPt" );
		INFOC( "Min Pair Pt = " << minPt );
	}


	virtual void makeChannel( string p ){
		INFOC( "{===========================CHANNEL=======================" );
		book->cd();
		string channel = config.get<string>( p +":name" );
		
		double BR      = config.get<double>( p + ":br", 1.0 );
		double uncBR   = config.get<double>( p + ":ebr", 1.0 );
		double dNdy    = config.get<double>( p + ":dndy", 1.0 );
		double uncdNdy = config.get<double>( p + ":redndy", 1.0 );	//relative error

		
		INFOC( " name  : " << channel );
		INFOC( " BR    : " << BR << " +/- " << uncBR );
		INFOC( " dNdy  : " << dNdy << " +/- " << uncdNdy );
		

		TH2 *hFullAcc = nullptr;
		TH2 *hAccCut  = nullptr;
		if ( "ccbar_mumu" == channel ){
			hFullAcc = get<TH2>( "FullAcc_dNdM_pT", channel );//(get<TH2>( "FullAcc_dNdM_pT_" + channel, channel ))->ProjectionX( ("FullAcc_dNdM_" + channel).c_str() );
			hAccCut  = get<TH2>( "AccCut_wdNdM_pT", channel );
			// hFullAcc->Scale( 1.0 / 0.7 );
			// BR = hFullAcc->GetXaxis()->GetBinWidth(1);
		}
		else {
			hFullAcc = get<TH2>( "PairCut_dNdM_pT", channel );
			hAccCut  = get<TH2>( "AccCut1_dNdM_pT", channel );
		}

		if ( nullptr == hFullAcc || nullptr == hAccCut){
			ERRORC( "Some Null Histos hFullAcc=" << hFullAcc << ", hAccCut=" << hAccCut << " SKIPPING" );
			return;
		}

		double IFA = hFullAcc->Integral();
		double IAC = hAccCut->Integral();
		double NFA = hFullAcc->GetEntries();
		double NAC = hAccCut->GetEntries();
		INFOC( "I(FullAcc) = " << IFA << ", N=" << NFA );
		INFOC( "I(AccCut)  = " << IAC << ", N=" << NAC );


		double dNdy_low  = dNdy - ( dNdy * uncdNdy );
		double dNdy_high = dNdy + ( dNdy * uncdNdy );
		double BR_low    = BR - uncBR;
		double BR_high   = BR + uncBR;

		INFOC( "IFA=" << IFA << ", dNdy=" << dNdy << ", BR=" << BR );
		double scale_factor = (1.0 / IFA) * dNdy * BR;
		double scale_factor_low = (1.0 / IFA) * dNdy_low * BR_low;
		double scale_factor_high = (1.0 / IFA) * dNdy_high * BR_high;

		book->addClone( "FullAcc_pT_" + channel, hFullAcc );
		book->addClone( "AccCut_pT_" + channel, hAccCut );
		TH2* hScaled = (TH2*)book->addClone( "Scaled_pT_" + channel, hAccCut );
		TH2* hScaledLow = (TH2*)book->addClone( "ScaledLow_pT_" + channel, hAccCut );
		TH2* hScaledHigh = (TH2*)book->addClone( "ScaledHigh_pT_" + channel, hAccCut );


		int rebin = config.getInt( "REBIN" );
		if ( rebin > 1 ){
			hScaled->Rebin( rebin );
			INFOC( "REBINNING by " << rebin );
		}

		INFOC( "Full Scale Factor = " << scale_factor );
		hScaled->Scale( scale_factor ); // scale by bin-width also
		hScaledLow->Scale( scale_factor_low );
		hScaledHigh->Scale( scale_factor_high );

		// for ( int i = 1; i <= hScaled->GetNbinsX(); i++ ){
		// 	double nomv = hScaled->GetBinContent( i );
		// 	double hiv  = hScaledHigh->GetBinContent( i );
		// 	hScaled->SetBinError( i, hiv - nomv );
		// }

		int b1 = hScaled->GetYaxis()->FindBin(minPt);
		TH1 * h1Scaled = hScaled->ProjectionX( ("Scaled_" + channel).c_str(), b1, -1 );
		if ( "ccbar_mumu" == channel ){
			h1Scaled->Scale( hFullAcc->GetXaxis()->GetBinWidth(1), "width" );
		} else {
			h1Scaled->Scale( 1.0, "width" );
		}

		if ( nullptr == hsum ){
			hsum = (TH1*)h1Scaled->Clone( "Scaled_sum" );
		} else {
			hsum->Add( h1Scaled );
		}

		INFOC( "}===========================CHANNEL=======================" );
	}


	virtual void make(){
		vector<string> paths = config.childrenOf( nodePath + ".ActiveChannels", "ActiveChannel" );
		INFOC( "Found " << paths.size() << " ActiveChannels" );
		for ( string p : paths ){
			INFOC( "Scaling ActiveChannel[" << config.get<string>( p+":name" ) << "] @ " << p );
			makeChannel( p );
		}

		
		




	} // make

protected:


};





#endif