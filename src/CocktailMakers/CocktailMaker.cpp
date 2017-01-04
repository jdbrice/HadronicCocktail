#include "TaskRunners/CocktailMaker.h"

void CocktailMaker::initialize(){
	DEBUG( classname(), "Initialize" );

	INFO( classname(), "Initialize RANDOM seed" );
	gRandom = new TRandom3();
	gRandom->SetSeed( 0 );

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
			book->clone( "", "pT", name, "parent_pT" );
			book->clone( "", "eta", name, "parent_eta" );
			book->clone( "", "phi", name, "parent_phi" );
			book->clone( "", "mass", name, "parent_mass" );
			book->clone( "", "mass", name, "parent_sampledMass" );
			book->clone( "", "mass", name, "parent_rawRecoMass" );
			book->clone( "", "mass_vs_pT", name, "parent_rawRecoMass_vs_pT" );
			book->clone( "", "mass", name, "parent_recoMass" );
			book->clone( "", "mass_vs_pT", name, "parent_recoMass_vs_pT" );

			book->clone( "", "pT", name, "l1_pT" );
			book->clone( "", "eta", name, "l1_eta" );
			book->clone( "", "phi", name, "l1_phi" );

			book->clone( "", "pT", name, "l2_pT" );
			book->clone( "", "eta", name, "l2_eta" );
			book->clone( "", "phi", name, "l2_phi" );
		} // if makeQA
	}

	if ( makeQA ){
		book->clone( "", "mass", "", "recoMass" );
		book->clone( "", "mass_vs_pT", "", "recoMass_vs_pT" );
	}

	// activeChannels = config.getStringVector( nodePath + ".ActiveChannels" );
	INFO( classname(), "Found " << activeChannels.size() << " active decay " << plural( activeChannels.size(), "channel", "channels" ) );

	for ( string name : activeChannels ){
		INFO( classname(), "Loading DecayChannelInfo for " << name << " from " <<  config.q( nodePath + ".DecayChannels{name==" + name + "}" ) );
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
	momShape = shared_ptr<TF1>( new TF1( "momShape", CrystalBall2, -1, 1, 7 ) );
	momShape->SetParameters(1., -1e-3, 0.01, 1.29, 1.75, 2.92, 1.84);


}


void CocktailMaker::make(){
	INFO( classname(), "" );

	

	for ( string name : activeChannels ){
		
		int N = namedN[ name ];
		TaskProgress tp( "Generating " + ts(N) + " " + name , N );
		int i = 0;
		tp.showProgress( i );
		i = 1;
		while ( i < N ){
			
		
			TLorentzVector lv = namedPlcSamplers[name].sample();
			
			if ( !parentFilter.pass( lv ) ) continue;

			
			namedPlcDecayers[ name ].decay( lv );

			TLorentzVector l1lv = namedPlcDecayers[ name ].getLepton1().lv;
			TLorentzVector l2lv = namedPlcDecayers[ name ].getLepton2().lv;

			if ( !daughterFilter.pass( l1lv ) || !daughterFilter.pass(l2lv) ) continue;

			tp.showProgress( i );

			postDecay( name, lv, namedPlcDecayers[ name ] );
			i++;
		}
		cout << "" << endl;

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
		book->get( "parent_pT", _name )->Fill( _parent.Pt() );
		book->get( "parent_eta", _name )->Fill( _parent.Eta() );
		book->get( "parent_phi", _name )->Fill( _parent.Phi() );
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