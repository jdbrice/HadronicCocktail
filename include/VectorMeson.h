#ifndef VECTOR_MESON_H
#define VECTOR_MESON_H

// RooBarb
#include "IObject.h"
#include "XmlConfig.h"
#include "Logger.h"

// ROOT

// Project

// STD
#include <limits>

/*
 *
 */
class VectorMeson : public IObject
{
public:
	virtual const char* classname() const { return "VectorMeson"; }
	
	/* Default Ctor
	 *
	 */
	VectorMeson() {}

	/*
	 *
	 */
	VectorMeson( XmlConfig &_cfg, string _nodePath ){
		setupFromConfig( _cfg, _nodePath );
	}

	/*
	 *
	 */
	~VectorMeson() {}
	
	/*
	 *
	 */
	void setupFromConfig( XmlConfig &_cfg, string _nodePath ){
		this->name = _cfg.getString( _nodePath + ":name" );
	}
	/*
	 *
	 */
	void setup(  );


	static int codeFor( string _name ){
		if( "pi0"           == _name ) return 111;
		if( "eta"           == _name ) return 221;
		if( "rho"           == _name ) return 113;
		if( "omega"         == _name ) return 223;
		if( "phi"           == _name ) return 333;
		if( "etaprime"      == _name ) return 331;
		if( "jpsi"          == _name ) return 443;
		if( "psi"           == _name ) return 100443;
		if( "virtualphoton" == _name ) return 0;
		return std::numeric_limits<int>::quiet_NaN();
	}

	static string nameFor( int _code ){
		if ( 111    == _code ) return "pi0";
		if ( 221    == _code ) return "eta";
		if ( 113    == _code ) return "rho";
		if ( 223    == _code ) return "omega";
		if ( 333    == _code ) return "phi";
		if ( 331    == _code ) return "etaprime";
		if ( 443    == _code ) return "jpsi";
		if ( 100443 == _code ) return "psi";
		if ( 0      == _code ) return "virtualphoton";
		return "";
	}


protected:
	string name;
	int mcCode;




};



#endif