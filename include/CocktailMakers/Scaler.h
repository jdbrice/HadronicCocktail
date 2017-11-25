#ifndef SCALER_H
#define SCALER_H

#include "HistoAnalyzer.h"

class Scaler : public HistoAnalyzer
{
protected:
	string channel;
	double BR   = 1.0;
	double dNdy = 1.0;
	double uncBR   = 0.0;
	double uncdNdy = 0.0;

public:
	virtual const char* classname() const { return "Scaler"; }
	Scaler() {}
	~Scaler() {}


	virtual void initialize(){
		HistoAnalyzer::initialize();
		INFOC( "" );
	}

		


	virtual void make(){

		channel = config["CHANNEL"];
		string sp = config.q( nodePath + ".ActiveChannels.ActiveChannel{name==" + channel + "}" );
		if ( config.exists( sp ) && config.exists( sp + ":br" ) && config.exists( sp + ":dndy" ) ){
			BR      = config.getDouble( sp + ":br", 1.0 );
			uncBR   = config.getDouble( sp + ":ebr", 1.0 );
			dNdy    = config.getDouble( sp + ":dndy", 1.0 );
			uncdNdy = config.getDouble( sp + ":redndy", 1.0 );	//relative error

			INFOC( "===========================CHANNEL=======================" );
			INFOC( " name  : " << channel );
			INFOC( " BR    : " << BR << " +/- " << uncBR );
			INFOC( " dNdy  : " << dNdy << " +/- " << uncdNdy );
			INFOC( "===========================CHANNEL=======================" );
		} else {
			ERRORC( "cannot find scaling info for " << channel );
			return;
		}


		book->cd();


		TH1 *hFullAcc = nullptr;
		TH1 *hAccCut = nullptr;
		if ( "ccbar_mumu" == channel ){
			hFullAcc = get<TH1>( "FullAcc_dNdM", "AccCut" );//(get<TH2>( "FullAcc_dNdM_pT_" + channel, "AccCut" ))->ProjectionX( ("FullAcc_dNdM_" + channel).c_str() );
			hAccCut  = get<TH1>( "AccCut_wdNdM", "AccCut" );
			hAccCut->Scale(10);
		}
		else {
			hFullAcc = get<TH1>( "RapCut_dNdM", "AccCut" );
			hAccCut  = get<TH1>( "AccCut_dNdM", "AccCut" );
		}

		if ( nullptr == hFullAcc || nullptr == hAccCut ){
			ERRORC( "NULL hitograms" );
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


		double scale_factor = (1.0 / IFA) * dNdy * BR;
		double scale_factor_low = (1.0 / IFA) * dNdy_low * BR_low;
		double scale_factor_high = (1.0 / IFA) * dNdy_high * BR_high;

		book->addClone( "FullAcc_" + channel, hFullAcc );
		book->addClone( "AccCut_" + channel, hAccCut );
		TH1* hScaled = book->addClone( "Scaled_" + channel, hAccCut );
		TH1* hScaledLow = book->addClone( "ScaledLow_" + channel, hAccCut );
		TH1* hScaledHigh = book->addClone( "ScaledHigh_" + channel, hAccCut );


		int rebin = config.getInt( "REBIN" );
		if ( rebin > 1 ){
			hScaled->Rebin( rebin );
			INFOC( "REBINNING by " << rebin );
		}

		INFOC( "Full Scale Factor = " << scale_factor );
		hScaled->Scale( scale_factor, "width" ); // scale by bin-width also
		hScaledLow->Scale( scale_factor_low, "width" );
		hScaledHigh->Scale( scale_factor_high, "width" );

		for ( int i = 1; i <= hScaled->GetNbinsX(); i++ ){
			double nomv = hScaled->GetBinContent( i );
			double hiv  = hScaledHigh->GetBinContent( i );
			hScaled->SetBinError( i, hiv - nomv );
		}




	} // make

protected:


};





#endif