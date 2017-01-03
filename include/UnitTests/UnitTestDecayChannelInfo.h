#ifndef UT_DECAY_CHANNEL_INFO_H
#define UT_DECAY_CHANNEL_INFO_H


// RooBarb
#include "TaskRunner.h"
#include "UnitTest/UnitTest.h"

#include "DecayChannelInfo.h"

class UnitTestDecayChannelInfo : public TaskRunner
{
public:
	virtual const char* classname() const { return "UnitTestDecayChannelInfo"; }
	UnitTestDecayChannelInfo(){}
	~UnitTestDecayChannelInfo(){}


	virtual void initialize(){
		
	}



	virtual void make(){

		UnitTest::start( config[ nodePath + ":subject" ] );

		DecayChannelInfo dci( config, nodePath + ".DecayChannel{name==omega_pi0ee_dalitz}" );

		UT_TEST( "DecayChannel name", UT_EQ( dci.name, "omega_pi0ee_dalitz" ) );

		DecayChannelInfo dci2 = DecayChannelInfo::get( config, nodePath, "omega_mumu" );
		UT_TEST( "DecayChannel name", UT_EQ( dci2.name, "omega_mumu" ) );


		INFO( classname(), UnitTest::summary() );

		config.toXmlFile( "UnitTestDecayChannelInfo_out.xml" );
	}
	
};


#endif