#ifndef FUNCTION_LIBRARY_H
#define FUNCTION_LIBRARY_H

// RooBarb
#include "IObject.h"
#include "XmlConfig.h"
#include "XmlFunction.h"
#include "Logger.h"
using namespace jdb;


// ROOT
#include "TF1.h"

// STL

// Project


// Global function definitions to work easily with ROOT
double TFormula_BreitWigner( double *x, double *par );
double TFormula_MasslesPS( double *x, double *par );


class FunctionLibrary : public IObject
{
public:
	virtual const char* classname() const { return "FunctionLibrary"; }
	FunctionLibrary() {}
	~FunctionLibrary() {}


	void loadFunction( XmlConfig &_cfg, string _nodePath, string _name = "" ){
		INFO( classname(), "Loading from " << _nodePath );
		XmlFunction xf1;
		xf1.set( _cfg, _nodePath );
		f1s.push_back( xf1.getTF1() );

		INFO( classname(), "Compiled: " << f1s[f1s.size()-1]->GetFormula()->GetExpFormula() );
	}

	void loadAll( XmlConfig &_cfg, string _nodePath ){
		INFO( classname(), "Loading all Functions @ " << _nodePath );
		vector<string> paths = _cfg.childrenOf( _nodePath, "TF1" );
		for ( string path : paths ){
			loadFunction( _cfg, path );
		}
	}


protected:

	static int tf1_instance_count;
	vector< shared_ptr<TF1> > f1s;

	
};



#endif