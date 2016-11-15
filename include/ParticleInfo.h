#ifndef PARTICLE_INFO_H
#define PARTICLE_INFO_H

// RooBarb
#include "IObject.h"
#include "XmlConfig.h"

// STL
#include <string>
#include <limits>

// ROOT

// Project

class ParticleInfo : public IObject
{
public:
	virtual const char* classname() const { return "ParticleInfo"; }
	ParticleInfo() {
		DEBUG( classname(), "" );
		this->name   = "NA";
		this->mcId = 0;
		this->mass   = 0;
		this->width  = 0;
	}

	ParticleInfo( string _name, int _mcId, double _mass, double _width ){
		DEBUG( classname(), "(name=\""<< _name << "\", mcId=" << _mcId << ", mass=" << _mass << ", width=" << _width << ")" );
		this->name   = _name;
		this->mcId = _mcId;
		this->mass   = _mass;
		this->width  = _width;	
	}

	ParticleInfo( XmlConfig &_cfg, string _nodePath ){
		DEBUG( classname(), "(cfg=" << _cfg.getFilename() << ", nodePath=" << _nodePath << ")" );
		this->name  = _cfg.getString( _nodePath + ":name", "NA" );
		this->mcId  = _cfg.getInt(    _nodePath + ":mcId", 0 );
		this->mass  = _cfg.getDouble( _nodePath + ":mass", 0 );
		this->width = _cfg.getDouble( _nodePath + ":width", 0 );
	}



	~ParticleInfo() {}

	string name;
	int mcId;
	double mass;
	double width;

protected:

	
};


#endif