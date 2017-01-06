#include "TaskRunners/CocktailMaker.h"

void CocktailMaker::initialize(){
	DEBUG( classname(), "Initialize" );

	int seed = config.getInt( "SEED", 0 );
	INFO( classname(), "Initialize RANDOM SEED = " << seed );
	gRandom = new TRandom3();
	if ( jobIndex >= 0  ){
		seed = seed * ( jobIndex + 1 );
	}
	gRandom->SetSeed( seed );

	makeQA = config.getBool( nodePath + ".Make:QA", true );

	initializeHistoBook( config, nodePath, "" );
	book->cd();
	book->makeAll( config, nodePath + ".histograms" );

	maxN = 0;

	INFO( classname(), "Initializing Function Library" );
	funLib.loadAll( config, nodePath + ".FunctionLibrary" );
	INFO( classname(), "Initializing Particle Library" );
	plcLib.loadParticles( config, nodePath + ".ParticleLibrary" );

	vector<string> acPaths = config.childrenOf( nodePath + ".ActiveChannels" );
	INFO( classname(), "Searching " << acPaths.size() << plural( acPaths.size(), " path", " paths" ) << " for ActiveChannels" );
	for ( string p : acPaths ){
		INFO(classname(), "Searching " << p);
		string name = config.getXString( p + ":name" );
		activeChannels.push_back( name );

		namedN[ name ]      = config.getInt( p + ":n", 2 );
		namedWeight[ name ] = config.getDouble( p + ":weight", 1.0 );

		if ( namedN[ name ] > maxN )
			maxN = namedN[ name ];


		if ( makeQA ){
			book->cd(name);
			book->clone( "", "pT_vs_phi", name, "parent_pT_vs_phi" );
			book->clone( "", "kfParent", name, "kfParent" );
			book->clone( "", "pT", name, "parent_pT" );
			book->clone( "", "pX", name, "parent_pX" );
			book->clone( "", "pY", name, "parent_pY" );
			book->clone( "", "eta", name, "parent_eta" );
			book->clone( "", "phi", name, "parent_phi" );
			book->clone( "", "phi", name, "parent_phi_pass" );
			book->clone( "", "pT", name, "parent_pT_pass" );
			book->clone( "", "pX", name, "parent_pX_pass" );
			book->clone( "", "pY", name, "parent_pY_pass" );

			book->clone( "", "mass", name, "parent_mass" );
			book->clone( "", "mass", name, "parent_sampledMass" );
			book->clone( "", "mass", name, "parent_rawRecoMass" );
			book->clone( "", "mass_vs_pT", name, "parent_rawRecoMass_vs_pT" );
			book->clone( "", "mass", name, "parent_recoMass" );
			book->clone( "", "mass_vs_pT", name, "parent_recoMass_vs_pT" );

			book->clone( "", "pT", name, "l1_pT" );
			book->clone( "", "eta", name, "l1_eta" );
			book->clone( "", "phi", name, "l1_phi" );
			book->clone( "", "kfLepton1", name, "kfLepton1" );

			book->clone( "", "pT", name, "l2_pT" );
			book->clone( "", "eta", name, "l2_eta" );
			book->clone( "", "phi", name, "l2_phi" );
			book->clone( "", "kfLepton2", name, "kfLepton2" );
		} // if makeQA
	}

	if ( makeQA ){
		book->clone( "", "mass", "", "recoMass" );
		book->clone( "", "mass_vs_pT", "", "recoMass_vs_pT" );
	}

	// activeChannels = config.getStringVector( nodePath + ".ActiveChannels" );
	INFO( classname(), "Found " << activeChannels.size() << " active decay " << plural( activeChannels.size(), "channel", "channels" ) );

	for ( string name : activeChannels ){
		string path = config.q( nodePath + ".DecayChannels{name==" + name + "}" );
		INFO( classname(), "Loading DecayChannelInfo for " << name << " from " <<  path );
		if ( "" == path ){
			ERROR( classname(), "CANNOT FIND DecayInfo for " << name << " at " << quote( path ) );
			activeChannels.clear();
			break;
		}
		DecayChannelInfo dci( config, config.q( nodePath + ".DecayChannels{name==" + name + "}" ) );
		namedDecayInfo[ name ] = dci;
		
		ParticleDecayer pd( plcLib, funLib );
		pd.setDecayChannel( dci );

		namedPlcDecayers[ name ] = pd;

		ParticleSampler ps;
		ps.set( plcLib.get( dci.parentName ), funLib, histoLib );
		namedPlcSamplers[ name ] = ps;
	}

	INFO( classname(), "Initializing parent kinematic filter");
	parentFilter.load( config, nodePath + ".KinematicFilters.Parent" );
	daughterFilter.load( config, nodePath + ".KinematicFilters.Daughter" );


	gErrorIgnoreLevel = kError;

	momResolution = funLib.get( "pT_Resolution" );
	momShape      = funLib.get( "pT_Shape" );
	if ( nullptr == momShape ){
		ERROR( classname(), "MUST proved a function for the Momentum Smearing Shape with name " << quote( "pT_Shape" ) << ". Using default CrystalBall shape with made-up parameters" );
		momShape = shared_ptr<TF1>( new TF1( "momShape", CrystalBall2, -1, 1, 7 ) );
		momShape->SetParameters(1., -1e-3, 0.01, 1.29, 1.75, 2.92, 1.84);	
	}

	momShape->Write();
	momResolution->Write();
	


}


void CocktailMaker::make(){
	INFO( classname(), "" );

	

	for ( string name : activeChannels ){

		TH1D * hkfParent = (TH1D*)book->get( "kfParent", name );
		TH1D * hkfLepton1 = (TH1D*)book->get( "kfLepton1", name );
		TH1D * hkfLepton2 = (TH1D*)book->get( "kfLepton2", name );
		
		int N = namedN[ name ];
		TaskProgress tp( "Generating " + ts(N) + " " + name , N );
		int i = 0;
		tp.showProgress( i );
		i = 1;
		int t = 1;
		while ( i < N && t < N * 1000000 ){
			t++;
		
			TLorentzVector lv = namedPlcSamplers[name].sample();

			if ( makeQA ){
				book->get( "parent_phi", name )->Fill( lv.Phi() );
				book->get( "parent_pX", name  )->Fill( lv.Px() );
				book->get( "parent_pY", name  )->Fill( lv.Py() );
				book->get( "parent_pT", name  )->Fill( lv.Pt() );

				book->get( "parent_pT_vs_phi", name  )->Fill( lv.Phi(), lv.Pt() );
			}
			
			if ( parentFilter.fail( lv, hkfParent ) ) continue;

			
			namedPlcDecayers[ name ].decay( lv );

			TLorentzVector l1lv = namedPlcDecayers[ name ].getLepton1().lv;
			TLorentzVector l2lv = namedPlcDecayers[ name ].getLepton2().lv;

			if ( daughterFilter.fail( l1lv, hkfLepton1 ) || daughterFilter.fail(l2lv, hkfLepton2) ) continue;

			tp.showProgress( i );

			postDecay( name, lv, namedPlcDecayers[ name ] );
			i++;
		}
		cout << "Efficiency: " << (float)i / t << endl;

	}// active channel loop
	
}

void CocktailMaker::postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd ){
	TLorentzVector l1lv = _pd.getLepton1().lv;
	TLorentzVector l2lv = _pd.getLepton2().lv;


	// TLorentzVector r1lv;
	double ptRes = momResolution->Eval( l1lv.Pt() ) * 100.0;
	double rndCrystalBall = momShape->GetRandom();
	rl1lv.SetPtEtaPhiM( 
			l1lv.Pt() * (1 + rndCrystalBall * ptRes  ) ,
			l1lv.Eta(),
			l1lv.Phi(),
			l1lv.M() );

	// TLorentzVector r2lv;
	ptRes = momResolution->Eval( l1lv.Pt() ) * 100.0;
	rndCrystalBall = momShape->GetRandom();
	rl2lv.SetPtEtaPhiM( 
			l2lv.Pt() * (1 + rndCrystalBall * ptRes  ) ,
			l2lv.Eta(),
			l2lv.Phi(),
			l2lv.M() );

	rplv = rl1lv + rl2lv;


	if ( makeQA ){
		book->get( "parent_pT_pass", _name )->Fill( _parent.Pt() );
		book->get( "parent_pX_pass", _name )->Fill( _parent.Px() );
		book->get( "parent_pY_pass", _name )->Fill( _parent.Py() );
		book->get( "parent_eta", _name )->Fill( _parent.Eta() );
		book->get( "parent_phi_pass", _name )->Fill( _parent.Phi() );
		book->get( "parent_mass", _name )->Fill( _parent.M() );
		book->get( "parent_sampledMass", _name )->Fill( _pd.getSampledMass() );
		book->get( "parent_rawRecoMass", _name )->Fill( (l1lv + l2lv).M() );

		book->get( "l1_pT",  _name )->Fill( l1lv.Pt() );
		book->get( "l1_eta", _name )->Fill( l1lv.Eta() );
		book->get( "l1_phi", _name )->Fill( l1lv.Phi() );

		book->get( "l2_pT",  _name )->Fill( l2lv.Pt() );
		book->get( "l2_eta", _name )->Fill( l2lv.Eta() );
		book->get( "l2_phi", _name )->Fill( l2lv.Phi() );


		book->get( "mass", "" )->Fill( _pd.getSampledMass(), namedWeight[ _name ] );

		book->get( "parent_recoMass", _name )->Fill( rplv.M() );

		book->get( "recoMass", "" )->Fill( rplv.M() );
		book->get( "recoMass_vs_pT", "" )->Fill( rplv.M(), rplv.Pt() );
	}

}