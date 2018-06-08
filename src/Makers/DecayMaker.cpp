#include "Makers/DecayMaker.h"
#include "EfficiencyWeightF1D.h"

void DecayMaker::initialize(){
	INFOC( "Initialize" );

	int seed = config.getInt( "SEED", 0 );
	
	gRandom = new TRandom3();
	
	int jobIndex = config.getInt( "jobIndex" );
	if ( "all" != config.getString( "jobIndex" ) && jobIndex >= 0  ){
		seed = seed + ( jobIndex + 1 );
	}
	INFO( classname(), "Initialize RANDOM SEED = " << seed );
	gRandom->SetSeed( seed );

	makeQA = config.getBool( nodePath + ".Make:QA", true );
	makeTF1 = config.getBool( nodePath + ".Make:TF1", false );

	INFOC( "initializeHistoBook" );
	initializeHistoBook( config, nodePath, "" );
	

	INFOC( "Make histograms" );
	book->cd();
	book->makeAll( config, nodePath + ".histograms" );
	prepareHistos();

	maxN = 0;

	INFO( classname(), "Initializing Function Library" );
	funLib.loadAll( config, nodePath + ".FunctionLibrary" );
	INFO( classname(), "Initializing Particle Library" );
	plcLib.loadParticles( config, nodePath + ".ParticleLibrary" );

	vector<string> acPaths = config.childrenOf( nodePath + ".ActiveChannels" );
	INFO( classname(), "Searching " << acPaths.size() << plural( acPaths.size(), " path", " paths" ) << " for ActiveChannels" );
	
	for ( string p : acPaths ){
		INFO(classname(), "Searching " << p);
		string name = config.getString( p + ":name" );
		activeChannels.push_back( name );

		namedN[ name ]      = config.getInt( p + ":n", 2 );
		namedWeight[ name ] = config.getDouble( p + ":weight", 1.0 );

		if ( namedN[ name ] > maxN )
			maxN = namedN[ name ];
	}

	if ( makeQA )
		prepareQAHistos();

	INFO( classname(), "Found " << activeChannels.size() << " active decay " << plural( activeChannels.size(), "channel", "channels" ) );

	for ( string name : activeChannels ){
		string path = config.q( nodePath + ".DecayChannels{name==" + name + "}" );
		INFO( classname(), "Loading DecayChannelInfo for " << name << " from " <<  path );
		
		if ( "" == path ){
			ERROR( classname(), "CANNOT FIND DecayInfo for " << name << " at " << quote( path ) );
			// activeChannels.clear();
			continue;
		}

		DecayChannelInfo dci( config, path );
		namedDecayInfo[ name ] = dci;
		
		ParticleDecayer pd( plcLib, funLib );
		pd.setDecayChannel( dci );

		namedPlcDecayers[ name ] = pd;

		ParticleSampler ps;
		ps.set( plcLib.get( dci.parentName ), funLib, histoLib );
		namedPlcSamplers[ name ] = ps;
	}

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


	momSmearing = config.getBool( nodePath + ".MomentumSmearing", true );
	INFOC( "==========MOMETUM SMEARING=================" );
	INFOC( "" << bts( momSmearing ) );
	INFOC( "==========MOMETUM SMEARING=================" );


	gErrorIgnoreLevel = kError;

	if ( makeTF1 ){
		momShape->Write();
		momResolution->Write();

		for ( string name : activeChannels ){
			book->cd(name);
			namedPlcDecayers[ name ].writeDistributions();
			namedPlcSamplers[ name ].writeDistributions();
		}
	} // makeTF1

	book->cd();

}

void DecayMaker::prepareQAHistos(){

	book->cd();
	book->makeAll( config, nodePath + ".QAhistograms" );

	vector<string> acPaths = config.childrenOf( nodePath + ".ActiveChannels" );
	for ( string p : acPaths ){
		string name = config.getString( p + ":name" );
		book->cd(name);
		book->clone( "", "pT_vs_phi", name, "parent_pT_vs_phi" );
		book->clone( "", "pT_vs_weight", name, "parent_pT_vs_weight" );
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
		book->clone( "", "mass", name, "parent_weightedMass" );
		book->clone( "", "mass", name, "parent_sampledMass" );
		book->clone( "", "mass", name, "parent_rawRecoMass" );
		book->clone( "", "mass_vs_pT", name, "parent_rawRecoMass_vs_pT" );
		book->clone( "", "mass_vs_pT", name, "parent_mass_vs_l1_pT" );
		book->clone( "", "mass_vs_pT", name, "parent_mass_vs_l2_pT" );
		book->clone( "", "mass_vs_pT", name, "parent_mass_vs_lpT" );
		book->clone( "", "mass", name, "parent_recoMass" );
		book->clone( "", "mass_vs_pT", name, "parent_recoMass_vs_pT" );
		book->clone( "", "mass_vs_weight", name, "parent_mass_vs_weight" );

		book->clone( "", "pT", name, "l1_pT" );
		book->clone( "", "R_vs_pT", name, "l1_R_vs_pT" );
		book->clone( "", "eta", name, "l1_eta" );
		book->clone( "", "phi", name, "l1_phi" );
		book->clone( "", "kfLepton1", name, "kfLepton1" );
		book->clone( "", "pT_vs_weight", name, "l1_pT_vs_weight" );


		book->clone( "", "pT", name, "l2_pT" );
		book->clone( "", "eta", name, "l2_eta" );
		book->clone( "", "phi", name, "l2_phi" );
		book->clone( "", "kfLepton2", name, "kfLepton2" );
		book->clone( "", "pT_vs_weight", name, "l2_pT_vs_weight" );
	}

	book->clone( "", "mass", "", "recoMass" );
	book->clone( "", "mass_vs_pT", "", "recoMass_vs_pT" );
}

void DecayMaker::prepareHistos(){
	vector<string> states = { "FullAcc_", "RapCut_", "AccCut_" };
	vector<string> histos = { "dNdM", "dNdM_pT", "dNdM_pT_eff", "PtRc", "PtMc", "Eta", "rapidity", "Eta_vs_l2Eta" };
	vector<string> ls     = { "l1", "l2", "w" };

	for ( string s : states ){
		for ( string hn : histos ){
			book->clone( hn, s + hn );
			for ( string l : ls ){
				if ( "w" != l && ("dNdM" == hn || "dNdM_pT" == hn) ) continue;
				book->clone( hn, s + l + hn );
			}// ls
		} // histos
	} // states
}

void DecayMaker::make(){
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
		while ( i < N ){
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

			// if ( daughterFilter.fail( l1lv, hkfLepton1 ) || daughterFilter.fail(l2lv, hkfLepton2) ) continue;

			tp.showProgress( i );

			postDecay( name, lv, namedPlcDecayers[ name ] );
			i++;
		}
		cout << "Efficiency: " << (float)i / t << endl;

	}// active channel loop
}

void DecayMaker::postDecay( string _name, TLorentzVector &_parent, ParticleDecayer &_pd ){
	

	TLorentzVector &mclv = _parent ;
	TLorentzVector mclv1, mclv2;
	mclv1 = _pd.getLepton1().lv;
	mclv2 = _pd.getLepton2().lv;

	INFOC( "postDecay( _name=" << _name << " )" );

	// =============================== MOMENTUM SMEARING ===============================
	// TLorentzVector rclv, rclv1, rclv2;
	double ptRes = momResolution->Eval( mclv1.Pt() );
	double rndCrystalBall = 0.0;
	if ( true == momSmearing ) rndCrystalBall = gRandom->Gaus( 0, 1.0 );
	
	rclv1.SetPtEtaPhiM(  mclv1.Pt() * (1 + rndCrystalBall * ptRes  ), mclv1.Eta(), mclv1.Phi(), mclv1.M() );
	
	ptRes = momResolution->Eval( mclv2.Pt() );
	rndCrystalBall = 0.0;
	if ( true == momSmearing ) rndCrystalBall = gRandom->Gaus( 0, 1.0 );

	rclv2.SetPtEtaPhiM( mclv2.Pt() * (1 + rndCrystalBall * ptRes  ) , mclv2.Eta(), mclv2.Phi(), mclv2.M() );
	rclv = rclv1 + rclv2;
	// =============================== MOMENTUM SMEARING ===============================

	if ( false == makeHistos ) return;
	
	double w = 1.0; // could support weigthing, no use now
	fillState( "FullAcc_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

	if ( !ccol.has( "rapidity" ) || ccol["rapidity"]->inInclusiveRange( rclv.Rapidity() ) ){
		fillState( "RapCut_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

		// check the kinematic filters
		if ( daughterFilter.pass( rclv1, rclv2 ) && parentFilter.pass( rclv ) ){
			fillState( "AccCut_", mclv, rclv, mclv1, rclv1, mclv2, rclv2, w );

		} // PASS kinematic filters
	} // PASS rapidity cut on parent

	// TLorentzVector l1lv = _pd.getLepton1().lv;
	// TLorentzVector l2lv = _pd.getLepton2().lv;
	

	// // TLorentzVector r1lv;
	// double ptRes = momResolution->Eval( l1lv.Pt() ) * 100.0;
	// double rndCrystalBall = momShape->GetRandom();
	// rl1lv.SetPtEtaPhiM( 
	// 		l1lv.Pt() * (1 + rndCrystalBall * ptRes  ) ,
	// 		l1lv.Eta(),
	// 		l1lv.Phi(),
	// 		l1lv.M() );

	// // TLorentzVector r2lv;
	// ptRes = momResolution->Eval( l1lv.Pt() ) * 100.0;
	// rndCrystalBall = momShape->GetRandom();
	// rl2lv.SetPtEtaPhiM( 
	// 		l2lv.Pt() * (1 + rndCrystalBall * ptRes  ) ,
	// 		l2lv.Eta(),
	// 		l2lv.Phi(),
	// 		l2lv.M() );

	// rplv = rl1lv + rl2lv;



	// if ( makeQA ){
	// 	double weight = 1.0;
	// 	TLorentzVector plv = l1lv + l2lv;

	// 	TLorentzVector lllv = l1lv;
	// 	if ( l2lv.Pt() > l1lv.Pt() )
	// 		lllv = l2lv;
		
	// 	book->get( "parent_pT_pass", _name )->Fill( _parent.Pt() );
	// 	book->get( "parent_pX_pass", _name )->Fill( _parent.Px() );
	// 	book->get( "parent_pY_pass", _name )->Fill( _parent.Py() );
	// 	book->get( "parent_eta", _name )->Fill( _parent.Eta() );
	// 	book->get( "parent_phi_pass", _name )->Fill( _parent.Phi() );
	// 	book->get( "parent_mass", _name )->Fill( plv.M() );
	// 	book->get( "parent_mass_vs_l1_pT", _name )->Fill( plv.M(), l1lv.Pt() );
	// 	book->get( "parent_mass_vs_l2_pT", _name )->Fill( plv.M(), l2lv.Pt() );
	// 	book->get( "parent_mass_vs_lpT", _name )->Fill( plv.M(), lllv.Pt() );
	// 	book->get( "parent_weightedMass", _name )->Fill( plv.M(), weight );
		
	// 	book->get( "parent_sampledMass", _name )->Fill( _pd.getSampledMass() );
	// 	book->get( "parent_rawRecoMass", _name )->Fill( plv.M() );
	// 	book->get2D( "parent_rawRecoMass_vs_pT", _name )->Fill( plv.M(), plv.Pt() );
	// 	book->get2D( "parent_pT_vs_weight", _name )->Fill( plv.Pt(), weight );
	// 	book->get2D( "parent_mass_vs_weight", _name )->Fill( plv.M(), weight );

	// 	book->get( "l1_pT",  _name )->Fill( l1lv.Pt() );
	// 	book->get2D( "l1_pT_vs_weight",  _name )->Fill( l1lv.Pt(), weight );
	// 	book->get( "l1_eta", _name )->Fill( l1lv.Eta() );
	// 	book->get( "l1_phi", _name )->Fill( l1lv.Phi(), weight );

	// 	book->get( "l2_pT",  _name )->Fill( l2lv.Pt() );
	// 	book->get2D( "l2_pT_vs_weight",  _name )->Fill( l2lv.Pt(), weight );
	// 	book->get( "l2_eta", _name )->Fill( l2lv.Eta() );
	// 	book->get( "l2_phi", _name )->Fill( l2lv.Phi(), weight );

	// 	book->get( "l1_R_vs_pT", _name ) ->Fill( l1lv.Pt(), l1lv.Pt() / (0.3 * 0.5) );


	// 	book->get( "mass", "" )->Fill( _pd.getSampledMass(), weight );

	// 	book->get( "parent_recoMass", _name )->Fill( rplv.M(), weight );

	// 	book->get( "recoMass", "" )->Fill( rplv.M(), weight );
	// 	book->get2D( "recoMass_vs_pT", "" )->Fill( rplv.M(), rplv.Pt(), weight );
	// }
}

void DecayMaker::fillState( string _s, 
									TLorentzVector &_lvMc, TLorentzVector &_lvRc, 
									TLorentzVector &_lvMc1, TLorentzVector &_lvRc1, 
									TLorentzVector &_lvMc2, TLorentzVector &_lvRc2,
									double _w ){

	book->fill(_s + "dNdM", _lvRc.M() );
	book->fill( _s + "dNdM_pT", _lvRc.M(), _lvRc.Pt() );
	book->fill( _s + "wdNdM", _lvRc.M(), _w );
	book->fill( _s + "wdNdM_pT", _lvRc.M(), _lvRc.Pt(), _w );
	book->fill( _s + "PtRc", _lvRc.Pt() );
	book->fill( _s + "PtMc", _lvMc.Pt() );
	book->fill( _s + "l1PtRc", _lvRc1.Pt() );
	book->fill( _s + "l2PtRc", _lvRc2.Pt() );
	book->fill( _s + "l1PtMc", _lvMc1.Pt() );
	book->fill( _s + "l2PtMc", _lvMc2.Pt() );

	book->fill( _s + "Eta", _lvRc.Eta() );
	book->fill( _s + "Eta_vs_l2Eta", _lvRc.Eta(), _lvRc2.Eta() );
	book->fill( _s + "l1Eta", _lvRc1.Eta() );
	book->fill( _s + "l1Eta_vs_l2Eta", _lvRc1.Eta(), _lvRc2.Eta() );
	book->fill( _s + "l2Eta", _lvRc2.Eta() );
	book->fill( _s + "rapidity", _lvRc.Rapidity() );
	book->fill( _s + "l1rapidity", _lvRc1.Rapidity() );
	book->fill( _s + "l2rapidity", _lvRc2.Rapidity() );

}