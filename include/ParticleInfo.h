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
		this->Nd 	 = 2;
		this->lv.SetPtEtaPhiM( 0, 0, 0, this->mass );
	}

	ParticleInfo( string _name, int _mcId, double _mass, double _width, double _Nd ){
		DEBUG( classname(), "(name=\""<< _name << "\", mcId=" << _mcId << ", mass=" << _mass << ", width=" << _width << ", " << _Nd << ")" );
		this->name  = _name;
		this->mcId  = _mcId;
		this->mass  = _mass;
		this->width = _width;	
		this->Nd    = _Nd;
		this->lv.SetPtEtaPhiM( 0, 0, 0, this->mass );
	}

	ParticleInfo( XmlConfig &_cfg, string _nodePath ){
		INFO( classname(), "(cfg=" << _cfg.getFilename() << ", nodePath=" << _nodePath << ")" );
		this->name       = _cfg.getString( _nodePath + ":name", "NA" );
		this->mcId       = _cfg.getInt(    _nodePath + ":mcId", 0 );
		this->mass       = _cfg.getDouble( _nodePath + ":mass", 0 );
		this->width      = _cfg.getDouble( _nodePath + ":width", 0 );
		this->gamma2     = _cfg.getDouble( _nodePath + ":gamma2", 0 );
		this->invLambda2 = _cfg.getDouble( _nodePath + ":invLambda2", 0 );
		this->Nd         = _cfg.getInt(    _nodePath + ":Nd", 2 );
		this->lv.SetPtEtaPhiM( 0, 0, 0, this->mass );

		INFO( classname(), toString() );
	}

	string toString(){
		string str = "";
		str += this->name + "(" + ts(this->mcId) + ")" + "<m=";
		str += dts(this->mass) + ", w=";
		str += dts( this->width ) + ", G2=";
		str += dts( this->gamma2 ) + ", iL2=";
		str += dts( this->invLambda2 );
		str += ", Nd = " + ts( this->Nd );
		str += ", lv(P=" + dts(lv.P()) + "[";
		str += dts(lv.Px()) + "," + dts(lv.Py()) + "," + dts(lv.Pz()) + "], M=";
		str += dts( lv.M() ) + ")>";
		return str;
	}

	bool isLepton() {
		if ( abs(mcId) == 11 || abs(mcId) == 13 || abs(mcId) == 15 )
			return true;
		return false;
	}

	bool isPhoton(){
		if ( mcId == 22 )
			return true;
		return false;
	}



	~ParticleInfo() {}

	string name;
	int mcId          = 0.0;
	double mass       = 0.0;
	double width      = 0.0;
	double gamma2     = 0.0;
	double invLambda2 = 0.0;
	int    Nd         = 2.0;


	TLorentzVector lv;


protected:

	
};


#endif