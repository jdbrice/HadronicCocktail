#ifndef HISTOGRAM_LIBRARY_H
#define HISTOGRAM_LIBRARY_H



// ROOT
#include "TH1.h"

// STL
#include <vector>
#include <map>
#include <cmath>


// RooBarb
#include "IObject.h"
#include "XmlConfig.h"
#include "XmlHistogram.h"
#include "Logger.h"
using namespace jdb;

class HistogramLibrary : public IObject
{
public:
	virtual const char* classname() const { return "HistogramLibrary"; }
	HistogramLibrary() {}
	~HistogramLibrary() {}


	void load( XmlConfig &_cfg, string _nodePath, string _name = "" ){
		INFO( classname(), "Loading from " << _nodePath );

		XmlHistogram xh1;
		
		string name = _cfg.getXString( _nodePath + ":name" );
		INFO( "Loading Histogram " << name );

		xh1.load( _cfg, _nodePath, ":url", ":hName" );
		h1s.push_back( xh1.getTH1() );

		if ( "" != name ){
			h1sByName[ name ] = xh1.getTH1();
		}
	}


	void loadAll( XmlConfig &_cfg, string _nodePath ){
		INFO( classname(), "Loading all Histograms @ " << _nodePath );
		
		vector<string> paths = _cfg.childrenOf( _nodePath, "TH1" );
		for ( string path : paths ){
			load( _cfg, path );
		}
	}

	shared_ptr<TH1> get( string _name ){
		if ( h1sByName.count( _name ) > 0 ){
			return h1sByName[ _name ];
		} return nullptr;
	}

	void report(){
		for ( auto kv : h1sByName ){
			cout << kv.first << " : " << endl;
		}
	}

protected:
	vector< shared_ptr<TH1> > h1s;
	map<string, shared_ptr<TH1> > h1sByName;

	
};


#endif