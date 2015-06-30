#ifndef ASSETS_H
#define ASSETS_H

#include "FileInfo.h"
#include <string>
using std::string;
#include <map>
using std::map;
using std::pair;

class Asset
{
public:
	Asset();
	virtual ~Asset();

	virtual bool Load( const string& filename );
	virtual void Unload();
};

class Assets
{
public:
	static Assets& Instance();
	virtual ~Assets(){}

	template<typename T> T* Load( const string& filename )
	{
		T* result = nullptr;

		map<string,Asset*>::iterator it = m_mapAssets.find( filename );
		if( it != m_mapAssets.end() )
			result = (T*)it->second;

		if( result == nullptr )
		{
			result = new T();
			if( result->Load( filename ) )
			{
				m_mapAssets.insert( pair<string,Asset*>( filename, result ) );
			}
			else
			{
				delete result;
				result = nullptr;
			}
		}

		return result;
	}

	void Unload();

private:
	Assets(){}

	map<string,Asset*> m_mapAssets;
};

#endif