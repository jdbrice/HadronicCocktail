#ifndef HISTOGRAM_TBW_H
#define HISTOGRAM_TBW_H

// RooBarb
#include "TaskRunner.h"
#include "IHistoBookMaker.h"
using namespace jdb;

// STL
#include <vector>
#include <string>
using namespace std;

// Project
#include "DecayChannelInfo.h"
#include "FunctionLibrary.h"
#include "ParticleLibrary.h"
#include "HistogramLibrary.h"
#include "ParticleDecayer.h"
#include "ParticleSampler.h"
#include "KinematicFilter.h"
#include "EfficiencyWeight.h"


// ROOT
#include "TNamed.h"

class HistogramTBW : public TaskRunner, public IHistoBookMaker
{
protected:

	FunctionLibrary funLib;
	ParticleLibrary plcLib;
	HistogramLibrary histoLib;

	vector<string> activeParticles;
	map<string, ParticleSampler>  namedPlcSamplers;

	int maxN;

public:
	virtual const char* classname() const { return "HistogramTBW"; }
	HistogramTBW() {}
	~HistogramTBW() {}

	virtual void init( XmlConfig &_config, string _nodePath ){
		TaskRunner::init( _config, _nodePath );
		initialize();
	}

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

	virtual void initialize(){
		INFOC( "Initialize" );

		gRandom = new TRandom3();
		
		unsigned long long int seed = get_seed();
		INFOC( "Setting seed = " << seed );
		gRandom->SetSeed( seed );

		INFOC( "initializeHistoBook" );
		initializeHistoBook( config, nodePath, "" );
		

		INFOC( "Make histograms" );
		book->cd();
		book->makeAll( config, nodePath + ".histograms" );

		maxN = config.get<size_t>( "N", 1000 );

		INFO( classname(), "Initializing Function Library" );
		funLib.loadAll( config, nodePath + ".FunctionLibrary" );
		INFO( classname(), "Initializing Particle Library" );
		plcLib.loadParticles( config, nodePath + ".ParticleLibrary" );

		activeParticles = config.getStringVector( "particles" );

		for ( string name : activeParticles ){

			ParticleSampler ps;
			ps.set( plcLib.get( name ), funLib, histoLib );
			namedPlcSamplers[ name ] = ps;

			book->clone( "dNdPt", name+"_dNdPt" );
			// book->clone( "dNdEta", name+"_dNdEta" );
			book->clone( "dNdY", name+"_dNdY" );
			book->clone( "dNdPhi", name+"_dNdPhi" );
		}


		gErrorIgnoreLevel = kError;

		book->cd();
	}

protected:


	virtual void make() {

		// for ( size_t i = 0; i < maxN; i++){
			for ( string name : activeParticles ){
				// cout << "name=" << name << endl;
				// TLorentzVector lv = namedPlcSamplers[name].sample();
				
				// Pt
				TH1 * h = book->get( name+"_dNdPt" );
				for ( int i = 1; i <= h->GetXaxis()->GetNbins(); i++ ){
					double x = h->GetBinCenter( i );
					h->SetBinContent( i, namedPlcSamplers[name].evalPt( x ) );
				}

				// Rapidity
				h = book->get( name+"_dNdY" );
				for ( int i = 1; i <= h->GetXaxis()->GetNbins(); i++ ){
					double x = h->GetBinCenter( i );
					h->SetBinContent( i, namedPlcSamplers[name].evalRapidity( x ) );
				}

				// Phi
				h = book->get( name+"_dNdPhi" );
				for ( int i = 1; i <= h->GetXaxis()->GetNbins(); i++ ){
					double x = h->GetBinCenter( i );
					h->SetBinContent( i, namedPlcSamplers[name].evalPhi( x ) );
				}

				// book->fill( name + "_dNdPt", lv.Pt() );
				// book->fill( name + "_dNdEta", lv.Eta() );
				// book->fill( name + "_dNdY", lv.Rapidity() );
				// book->fill( name + "_dNdPhi", lv.Phi() );
			}
		// }
	}


	virtual void postMake() {
		TaskRunner::postMake();

		TNamed n( "config", config.toXml() );
		n.Write();
	}

	virtual void overrideConfig() {
		DEBUG( classname(), "" );

		if ( !config.exists( "jobIndex" ) || config.getInt( "jobIndex" ) == -1 ){
			config.set( "jobIndex", "all" );
		}
	}

	
};


#endif