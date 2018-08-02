#ifndef SCALER_H
#define SCALER_H

#include "HistoAnalyzer.h"

class Scaler : public HistoAnalyzer
{
protected:
	float minPt = 0.0;

	TH1 * hsum     = nullptr;
	TH1 * hsumLow  = nullptr;
	TH1 * hsumHigh = nullptr;

	float Ncoll = 1.0;

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

		INFOC( " checking if file exists: " << channel << " : " << bts( fileExists( channel ) ) );
		
		double BR      = config.get<double>( p + ":br", 1.0 );
		double uncBR   = config.get<double>( p + ":ebr", 1.0 );
		double dNdy    = config.get<double>( p + ":dndy", 1.0 );
		double uncdNdy = config.get<double>( p + ":redndy", 1.0 );	//relative error

		
		INFOC( " name  : " << channel );
		INFOC( " BR    : " << BR << " +/- " << uncBR );
		INFOC( " dNdy  : " << dNdy << " +/- " << uncdNdy );
		
		string accState = config.get<string>( "ACC", "AccCut1" );
		INFOC( "Using " << accState << " : NOTE: AccCut1 is NOMINAL" );

		TH2 *hFullAcc = nullptr;
		TH2 *hAccCut  = nullptr;
		if ( "ccbar_mumu" == channel ){
			hFullAcc = get<TH2>( "FullAcc_dNdM_pT", channel );//(get<TH2>( "FullAcc_dNdM_pT_" + channel, channel ))->ProjectionX( ("FullAcc_dNdM_" + channel).c_str() );
			hAccCut  = get<TH2>( accState + "_wdNdM_pT", channel );
			// hFullAcc->Scale( 1.0 / 0.7 );
			// BR = hFullAcc->GetXaxis()->GetBinWidth(1);
		} 
		else if ( "bbbar_mumu" == channel || "drellyan_mumu" == channel ){
			hFullAcc = get<TH2>( "FullAcc_dNdM_pT", channel );
			hAccCut  = get<TH2>( accState + "_dNdM_pT", channel );
		}
		else {
			hFullAcc = get<TH2>( "PairCut_dNdM_pT", channel );
			hAccCut  = get<TH2>( accState + "_dNdM_pT", channel );
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
		double scale_factor = (Ncoll / IFA) * dNdy * BR;
		double scale_factor_low = (Ncoll / IFA) * dNdy_low * BR_low;
		double scale_factor_high = (Ncoll / IFA) * dNdy_high * BR_high;

		book->addClone( "FullAcc_pT_" + channel, hFullAcc );
		book->addClone( "AccCut_pT_" + channel, hAccCut );
		TH2* hScaled = (TH2*)book->addClone( "Scaled_pT_" + channel, hAccCut );
		TH2* hScaledLow = (TH2*)book->addClone( "ScaledLow_pT_" + channel, hAccCut );
		TH2* hScaledHigh = (TH2*)book->addClone( "ScaledHigh_pT_" + channel, hAccCut );


		int rebin = config.getInt( "REBIN" );
		if ( rebin > 1 ){
			hScaled->RebinX( rebin );
			hScaledLow->RebinX( rebin );
			hScaledHigh->RebinX( rebin );
			INFOC( "REBINNING by " << rebin );
		}

		INFOC( "Full Scale Factor = " << scale_factor );
		hScaled->Scale( scale_factor ); // scale by bin-width also
		hScaledLow->Scale( scale_factor_low );
		hScaledHigh->Scale( scale_factor_high );



		int b1 = hScaled->GetYaxis()->FindBin(minPt);
		TH1 * h1Scaled      = hScaled->ProjectionX( ("Scaled_" + channel).c_str(), b1, -1 );
		TH1 * h1ScaledLow   = hScaledLow->ProjectionX( ("ScaledLow_" + channel).c_str(), b1, -1 );
		TH1 * h1ScaledHigh  = hScaledHigh->ProjectionX( ("ScaledHigh_" + channel).c_str(), b1, -1 );
		
		// if ( "ccbar_mumu" == channel || "bbbar_mumu" == channel ){
		// 	h1Scaled    ->Scale( hFullAcc->GetXaxis()->GetBinWidth(1), "width" );
		// 	h1ScaledLow ->Scale( hFullAcc->GetXaxis()->GetBinWidth(1), "width" );
		// 	h1ScaledHigh->Scale( hFullAcc->GetXaxis()->GetBinWidth(1), "width" );
		// } else {
			h1Scaled    ->Scale( 1.0, "width" );
			h1ScaledLow ->Scale( 1.0, "width" );
			h1ScaledHigh->Scale( 1.0, "width" );
		// }

		if ( nullptr == hsum ){
			hsum     = (TH1*)h1Scaled    ->Clone( "Scaled_sum" );
			hsumLow  = (TH1*)h1ScaledLow ->Clone( "ScaledLow_sum" );
			hsumHigh = (TH1*)h1ScaledHigh->Clone( "ScaledHigh_sum" );

			hsum->Reset();
			hsumLow->Reset();
			hsumHigh->Reset();
		}
		
		hsum    ->Add( h1Scaled );
		hsumLow ->Add( h1ScaledLow );
		hsumHigh->Add( h1ScaledHigh );
		

		for ( int i = 1; i <= hScaled->GetNbinsX(); i++ ){
			double nomv = h1Scaled->GetBinContent( i );
			double hiv  = h1ScaledHigh->GetBinContent( i );
			h1Scaled->SetBinError( i, hiv - nomv );
		}

		INFOC( "}===========================CHANNEL=======================" );
	}


	virtual void make(){
		vector<string> paths = config.childrenOf( nodePath + ".ActiveChannels", "ActiveChannel" );
		INFOC( "Found " << paths.size() << " ActiveChannels" );

		Ncoll = config.get<float>( "Ncoll", 1.0 );
		INFOC( "Ncoll = " << Ncoll );

		for ( string p : paths ){
			INFOC( "Scaling ActiveChannel[" << config.get<string>( p+":name" ) << "] @ " << p );
			makeChannel( p );

			if ( nullptr != hsum && nullptr != hsumHigh ){
				for ( int i = 1; i <= hsum->GetNbinsX(); i++ ){
					double nomv = hsum->GetBinContent( i );
					double hiv  = hsumHigh->GetBinContent( i );
					hsum->SetBinError( i, hiv - nomv );
				}
			}
			
		}
	} // make

protected:


};





#endif