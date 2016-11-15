#ifndef DECAY_CHANNEL_INFO_H
#define DECAY_CHANNEL_INFO_H


class DecayChannelInfo : public IObject
{
public:
	virtual const char* classname() const { return "DecayChannelInfo"; }
	DecayChannelInfo() {}
	DecayChannelInfo( string _name, string _parent, vector<string> _productNames ) {
		this->name         = _name;
		this->parentName   = _parent;
		this->productNames = _productNames;
	}

	DecayChannelInfo( string _name, string _parent, string _d1Name, string _d2Name, string _d3Name ="" ) {
		this->name       = _name;
		this->parentName = _parent;
		this->productNames.push_back( _d1Name );
		this->productNames.push_back( _d2Name );
		if ( "" != _d3Name ){
			this->productNames.push_back( _d3Name );
		}
	}

	DecayChannelInfo( XmlConfig &_cfg, string _nodePath ){
		loadInfo( _cfg, _nodePath );
	}

	void loadInfo( XmlConfig &_cfg, string _nodePath ){
		DEBUG( classname(), "(_cfg=" << _cfg.getFilename() << ", nodePath=" << _nodePath << ")" );
		this->name       = _cfg.getString( _nodePath + ":name", "NA" );
		this->parentName = _cfg.getString( _nodePath + ":parent", "NA" );

		vector<string> paths = _cfg.childrenOf( _nodePath, "DecayProduct" );
		for ( string path : paths ){
			string prodName = _cfg.getString( path + ":name" );
			INFO( classname(), "Adding Product " << prodName );
			productNames.push_back( prodName );
		}
	}

	~DecayChannelInfo() {}
	

	string name;
	string parentName;
	vector<string> productNames;

protected:

};


#endif