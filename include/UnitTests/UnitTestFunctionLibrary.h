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
		f1lib.loadAll( config, nodePath );

		TF1 * ftest = new TF1( "test", "BreitWigner" );
		// TF1 * ftest2 = new TF1( "ftest2", "" );
		// 
		// 
		FunctionLibrary f2lib = f1lib;


		TF1 * ftest2 = new TF1( "test2", "BreitWigner" );


		INFO( classname(), "f1lib BreitWigner = " << f1lib.get( "BreitWigner" ) );
		INFO( classname(), "f2lib BreitWigner = " << f2lib.get( "BreitWigner" ) );


		INFO( classname(), UnitTest::summary() );

		config.toXmlFile( "UnitTestFunctionLibrary_out.xml" );
	}
	
};


#endif