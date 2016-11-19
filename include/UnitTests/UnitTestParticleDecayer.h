#ifndef UT_PARTICLE_DECAYER_INFO_H
#define UT_PARTICLE_DECAYER_INFO_H


// RooBarb
#include "TaskRunner.h"
#include "UnitTest/UnitTest.h"

#include "ParticleDecayer.h"

#include "TRandom.h"
#include "TRandom3.h"

class UnitTestParticleDecayer : public TaskRunner
{
public:
	virtual const char* classname() const { return "UnitTestParticleDecayer"; }
	UnitTestParticleDecayer(){}
	~UnitTestParticleDecayer(){}


	virtual void initialize(){
		
	}



	virtual void make(){

		UnitTest::start( config[ nodePath + ":subject" ] );


		FunctionLibrary funLib;
		// funLib.loadAll( config, nodePath + ".FunctionLibrary" );
		ParticleLibrary plcLib;
		plcLib.loadParticles( config, nodePath + ".ParticleLibrary" );

		ParticleDecayer pd( plcLib, funLib );
		DecayChannelInfo dci_dalitz( config, nodePath + ".DecayChannels.DecayChannel[0]" );
		DecayChannelInfo dci_twoBody( config, nodePath + ".DecayChannels.DecayChannel[1]" );

		pd.setDecayChannel( dci_twoBody );

		INFO( classname(), "\n" << pd.toString(1) );

		TLorentzVector lv;
		lv.SetXYZM( 10.0, 1.0, 0.0, 1.2 );
		pd.decay( lv );
		INFO( classname(), "\n" << pd.toString(1) );

		pd.setDecayChannel( dci_dalitz );

		// Taken from Shuai's code output
		// for omega to pi0 e+e-
		lv.SetXYZM( -0.767506, 0.839626, 0.691092, 0.78265 );
		double eeMass = 0.00556135;

		gRandom = new TRandom3();
		gRandom->SetSeed( 1 );
		// INFO( classname(), "Energy Conserved : " << bts( pd.checkDalitzEnergyConservation( lv ) ) );
		pd.decay( lv );

		TLorentzVector plv = pd.getLepton1().lv + pd.getLepton2().lv + pd.getNeutral().lv;
		INFO( classname(), "lv( P=" << dts(plv.Px()) << "," << dts(plv.Py()) << "," << dts(plv.Pz()) << ", M=" << dts( plv.M() ) << ")" );

		UT_TEST( "3-body decay reconstruction, mass", UT_DEQ( lv.M(), plv.M(), 0.000001 ) );
		UT_TEST( "3-body decay reconstruction, momentum", UT_DEQ( lv.P(), plv.P(), 0.000001 ) );


		// TEST EVENT From Shuai's code - used this to fine-grain unit test while developing
		// Parent of Dalitz : -0.767506, 0.839626, 0.691092, 0.78265
		// eeMass = 0.00556135
		// e1 = 0.00278067
		// p1 = 0.00273332
		// betaSquare = 0.966229
		// lambda = 0.934665
		// costheta = 0.523013
		// sintheta = 0.852325
		// phi = 3.42128
		// sinphi = -0.276053
		// cosphi = -0.961142
		// costheta = -0.148225
		// phi = 4.2723
		// e3 = 0.402944
		// p3 = 0.379665
		// P[0] = -0.00223915, -0.000643114, 0.00142956
		// P[1] = 0.00223915, 0.000643114, -0.00142956
		// P[0] = 0.000120938, -0.00125276, -0.00242631
		// P[1] = -0.000120938, 0.00125276, 0.00242631
		// fProducts[0] : ( P=0.00273332[0.000120938, -0.00125276, -0.00242631], M=0.000511)
		// fProducts[1] : ( P=0.00273332[-0.000120938, 0.00125276, 0.00242631], M=0.000511)
		// d1 : ( P=0.241871[-0.0706378, 0.203119, 0.1107], M=0.000511)
		// d2 : ( P=0.764498[-0.226622, 0.636755, 0.357271], M=0.000511)
		// d3 : ( P=0.520494[-0.470246, -0.000248552, 0.22312], M=0.134977)


		INFO( classname(), "\n" << pd.toString(1) );

		INFO( classname(), UnitTest::summary() );

		config.toXmlFile( "UnitTestParticleDecayer_out.xml" );
	}
	
};


#endif