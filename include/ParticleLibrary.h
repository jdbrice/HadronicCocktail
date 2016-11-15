#ifndef PARTICLE_LIBRARY_H
#define PARTICLE_LIBRARY_H

// RooBarb
#include "IObject.h"
#include "XmlConfig.h"

// STL
#include <string>
#include <limits>
#include <map>
#include <memory>

// ROOT

// Project
#include "ParticleInfo.h"


class ParticleLibrary : public IObject
{
public:
	virtual const char* classname() const { return "ParticleLibrary"; }
	ParticleLibrary() {}
	~ParticleLibrary() {}

	void loadParticles( XmlConfig &_cfg, string _nodePath ){

		vector<string> paths = _cfg.childrenOf( _nodePath, "Particle" );
		for ( string path : paths ){
			loadParticle( _cfg, path );
		}
	}

	void loadParticle( XmlConfig &_cfg, string _nodePath ){
		shared_ptr<ParticleInfo> plc = shared_ptr<ParticleInfo>( new ParticleInfo( _cfg, _nodePath ) );
		plcs.push_back( plc );


		if ( plcsByName.count( plc->name ) > 0 ) {
			ERROR( classname(), "Duplicate particle name : " << plc->name );
		}
		plcsByName[ plc->name ] = plc;

		if ( plcsByMCId.count( plc->mcId ) > 0 ) {
			ERROR( classname(), "Duplicate particle MC Id : " << plc->mcId );
		}
		plcsByMCId[ plc->mcId ] = plc;


	}

	ParticleInfo get( string _name ){
		if ( plcsByName.count( _name ) > 0 ){
			return *(plcsByName[ _name ]);
		}
		return ParticleInfo();
	}

	ParticleInfo get( int _mcId ){
		if ( plcsByMCId.count( _mcId ) > 0 ){
			return *(plcsByMCId[ _mcId ]);
		}
		return ParticleInfo();
	}


protected:

	vector<shared_ptr<ParticleInfo>> plcs;
	map<string, shared_ptr<ParticleInfo>> plcsByName;
	map<int, shared_ptr<ParticleInfo>> plcsByMCId;
	
};



#endif