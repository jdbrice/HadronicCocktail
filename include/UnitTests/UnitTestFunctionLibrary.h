#ifndef UT_FUNCTION_LIBRARY_H
#define UT_FUNCTION_LIBRARY_H


// RooBarb
#include "TaskRunner.h"
#include "UnitTest/UnitTest.h"

#include "FunctionLibrary.h"

class UnitTestFunctionLibrary : public TaskRunner
{
public:
	virtual const char* classname() const { return "UnitTestFunctionLibrary"; }
	UnitTestFunctionLibrary(){}
	~UnitTestFunctionLibrary(){}


	virtual void initialize(){
		
	}



	virtual void make(){

		UnitTest::start( config[ nodePath + ":subject" ] );

		FunctionLibrary f1lib;
		f1lib.loadFunction( config, nodePath + ".TF1[0]" );

		TF1 * ftest = new TF1( "test", "TF1_BreitWigner(0)" );


		INFO( classname(), UnitTest::summary() );

		config.toXmlFile( "UnitTestFunctionLibrary_out.xml" );
	}
	
};


#endif