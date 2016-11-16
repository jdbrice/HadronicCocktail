#ifndef PARTICLE_INFO_H
#define PARTICLE_INFO_H

// RooBarb
#include "IObject.h"
#include "XmlConfig.h"
#include "Utils.h"
using namespace jdb;

// STL
#include <string>
#include <limits>

// ROOT
#include "TLorentzVector.h"
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

	string toString(){
		string str = "";
		str += this->name + "(" + ts(this->mcId) + ")" + "<m=";
		str += dts(this->mass) + ", w=";
		str += dts( this->width ) + ", lv(P=";
		str += dts(lv.Px()) + "," + dts(lv.Py()) + "," + dts(lv.Pz()) + ", M=";
		str += dts( lv.M() ) + ")>";
		return str;
	}



	~ParticleInfo() {}

	string name;
	int mcId;
	double mass;
	double width;


	TLorentzVector lv;

protected:

	
};


#endif