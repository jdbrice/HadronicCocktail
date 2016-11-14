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
		this->name   = _cfg.getString( _nodePath + ":name", "NA" );
		this->mcCode = _cfg.getInt(    _nodePath + ":code" );
		this->width  = _cfg.getDouble( _nodePath + ":width", 0 );



	}
	/*
	 *
	 */
	void setup(  );





protected:
	string name;
	int mcCode;
	double width;
	MassDistribution massDist;





};



#endif