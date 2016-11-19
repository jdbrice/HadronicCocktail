#ifndef UT_HISTOGRAM_LIBRARY_H
#define UT_HISTOGRAM_LIBRARY_H


// RooBarb
#include "TaskRunner.h"
#include "UnitTest/UnitTest.h"

#include "HistogramLibrary.h"

class UnitTestHistogramLibrary : public TaskRunner
{
public:
	virtual const char* classname() const { return "UnitTestHistogramLibrary"; }
	UnitTestHistogramLibrary(){}
	~UnitTestHistogramLibrary(){}


	virtual void initialize(){
		
	}



	virtual void make(){

		UnitTest::start( config[ nodePath + ":subject" ] );

		HistogramLibrary histoLib;
		histoLib.loadAll( config, nodePath );

	

		INFO( classname(), UnitTest::summary() );

		config.toXmlFile( "UnitTestHistogramLibrary_out.xml" );
	}
	
};


#endif