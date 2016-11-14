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
double tf1_BreitWigner( double *x, double *par );



class FunctionLibrary : public IObject
{
public:
	virtual const char* classname() const { return "FunctionLibrary"; }
	FunctionLibrary() {}
	~FunctionLibrary() {}


	void loadFunction( XmlConfig &_cfg, string _nodePath, string _name = "" ){
		XmlFunction xf1;
		xf1.set( _cfg, _nodePath );
	}


protected:

	static int tf1_instance_count;
	vector< shared_ptr<TF1> > f1s;

	
};



#endif