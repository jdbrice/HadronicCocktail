#ifndef DECAY_CHANNEL_H
#define DECAY_CHANNEL_H


// RooBarb
#include "IObject.h"
#include "XmlConfig.h"

// STL
#include <string>
#include <limits>
#include <map>
#include <memory>

// ROOT

// Project


class DecayChannel : public IObject
{
public:
	virtual const char* classname() const { return "DecayChannel"; }
	DecayChannel() {}
	~DecayChannel() {}

	
	
};



#endif