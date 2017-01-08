#ifndef FUNCTION_LIBRARY_H
#define FUNCTION_LIBRARY_H

// ROOT
#include "TF1.h"
#include "TF2.h"

// STL
#include <vector>
#include <map>
#include <cmath>
#include <functional>
#include <memory>

// Project
#include "CintFunctionLibrary.h"


// RooBarb
#include "IObject.h"
#include "XmlConfig.h"
#include "XmlFunction.h"
#include "Logger.h"
using namespace jdb;

class FunctionLibrary : public IObject
{
public:
	virtual const char* classname() const { return "FunctionLibrary"; }
	FunctionLibrary() {}
	~FunctionLibrary() {}


	void loadFunction( XmlConfig &_cfg, string _nodePath, string _name = "" ){
		INFO( classname(), "Loading from " << _nodePath );
		
		bool foundBuiltin = loadBuiltin( _cfg, _nodePath );
		if ( foundBuiltin ) return;

		XmlFunction xf1;
		xf1.set( _cfg, _nodePath );
		f1s.push_back( xf1.getTF1() );

		string name = _cfg.getString( _nodePath + ":name" );
		INFO( "Loading Function " << name );
		if ( "" != name ){
			f1sByName[ name ] = xf1.getTF1();
		}

		INFO( classname(), "Compiled: " << f1s[f1s.size()-1]->GetFormula()->GetExpFormula() );
	}

	bool loadBuiltin( XmlConfig &_cfg, string _nodePath ){
		
		string &np = _nodePath;

		string formula = _cfg.getString( _nodePath + ":formula" );
		string name = _cfg.getString( _nodePath + ":name" );

		// Speial Cases - functions backed by C++ code
		if ( "TsallisBlastWave" == formula ){
			double m        = 0;	// gets overwritten by ParticleSampler
			double beta_max = _cfg.getDouble( np+":beta_max" , 0 );
			double temp     = _cfg.getDouble( np+":temp"     , _cfg.getDouble( np+":T" , 0 ) );
			double n        = _cfg.getDouble( np+":n"        , 0 );
			double q        = _cfg.getDouble( np+":q"        , 0 );
			double norm     = _cfg.getDouble( np+":norm"     , 1.00 );
			double ymin     = _cfg.getDouble( np+":ymin"     , -0.5 );
			double ymax     = _cfg.getDouble( np+":ymax"     , 0.5 );
			INFO( classname(), name << " : TBW<m=" << 0 << ",beta_max=" << beta_max << ",T=" << temp << ",n=" << n << ",q=" << q << ",norm=" << norm <<",ymin=" << ymin << ",ymax=" << ymax << ">");
			shared_ptr<TF1> f1 = shared_ptr<TF1>(  
									TsallisBlastWave( 	name.c_str(), m, beta_max, temp, n, q, norm, ymin, ymax )  
								);

			f1s.push_back( f1 );
			f1sByName[ name ] = f1;
			builtInByName[ name ] = formula;

			f1->SetRange( _cfg.getDouble( np+":min"     , 0.0 ), _cfg.getDouble( np+":max"     , 10.0 ) );

			return true;
		} else if ( "CrystalBall2" == formula ){

			double N     = _cfg.getDouble( np+":N"     , 0 );
			double mu    = _cfg.getDouble( np+":mu"    , 0 );
			double sig   = _cfg.getDouble( np+":sig"   , 0 );
			double n     = _cfg.getDouble( np+":n"     , 0 );
			double alpha = _cfg.getDouble( np+":alpha" , 0 );
			double m     = _cfg.getDouble( np+":m"     , 0 );
			double beta  = _cfg.getDouble( np+":beta"  , 0 );

			shared_ptr<TF1> f1 = shared_ptr<TF1>( new TF1( name.c_str(), CrystalBall2, -1, 1, 7 ) );
			f1->SetParameters( N, mu, sig, n, alpha, m, beta );
			f1->SetParNames( "N", "mu", "sig", "n", "alpha", "m", "beta" );
			INFO( classname(), name << " : DuobleCrystalBall<N=" << N << ",mu=" << mu << ",sig=" << sig << ",n=" << n << ",alpha=" << alpha << ",m=" << m << ",beta=" << beta << ">" );

			f1s.push_back( f1 );
			f1sByName[ name ] = f1;
			builtInByName[ name ] = formula;

			f1->SetRange( _cfg.getDouble( np+":min"     , -1.0 ), _cfg.getDouble( np+":max"     , 1.0 ) );
			return true;
		} else if ( "MassVacuumRho" == formula ) {

			double T 	= _cfg.getDouble( np+":temp", _cfg.getDouble( np+":T", 160 ) );
			double pT   = _cfg.getDouble( np+":pT", 0.1 );	// this gets overwritten but it lets me control which one is written out when debugging 
			// other params are picked up from the particle info

			shared_ptr<TF1> f1 = shared_ptr<TF1>( new TF1( name.c_str(), MassVacuumRho, 0, 10, 5 ) );	
			f1->SetParNames( "pT", "ml", "gamma0", "gamma2", "T" );
			// reasonable defaults, but everything but T gets overwritten
			f1->SetParameters( pT, 0.105, 0.1491, 4.55e-5, T);

			f1s.push_back( f1 );
			f1sByName[ name ] = f1;

			builtInByName[ name ] = formula;


			f1->SetRange( _cfg.getDouble( np+":min"     , 0.0 ), _cfg.getDouble( np+":max"     , 5.0 ) );	
			f1->SetNpx( _cfg.getInt( np+":Npx", 500 ) );

			return true;
		}


		return false; 
		
	}

	void loadAll( XmlConfig &_cfg, string _nodePath ){
		INFO( classname(), "Loading all Functions @ " << _nodePath );

		vector<string> paths = _cfg.childrenOf( _nodePath, "TF1" );
		for ( string path : paths ){
			loadFunction( _cfg, path );
		}
	}

	shared_ptr<TF1> get( string _name ){
		if ( f1sByName.count( _name ) > 0 ){
			return f1sByName[ _name ];
		} return nullptr;
	}

	string functionExpression( string _name ){
		if ( builtInByName.count( _name ) > 0 )
			return builtInByName[ _name ];
		if ( f1sByName.count( _name ) > 0 && f1sByName[ _name ] ){
			return f1sByName[ _name ]->GetExpFormula().Data();
		}
		return "";
	}

	shared_ptr<TF1> copy( string _name, string _new_name ){
		shared_ptr<TF1> orig = get(_name);
		shared_ptr<TF1> copied = shared_ptr<TF1>( new TF1( ) );
		if ( orig ){
			orig->Copy( (*copied) );
		} 
		copied->SetName( _new_name.c_str() );
		return copied;
	}


protected:
	vector< shared_ptr<TF1> > f1s;
	map<string, shared_ptr<TF1> > f1sByName;
	map<string, string> builtInByName;
	// map< string, std::function< double(double*,double*) > > builtInMap;

	
};



#endif