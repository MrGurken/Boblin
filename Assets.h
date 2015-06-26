#ifndef ASSETS_H
#define ASSETS_H

#include "SDL.h"
#include "SDL_mutex.h"
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

		SDL_LockMutex( m_sdlMutex );
		map<string,Asset*>::iterator it = m_mapAssets.find( filename );
		if( it != m_mapAssets.end() )
			result = it->second;
		SDL_UnlockMutex( m_sdlMutex );

		if( result == nullptr )
		{
			result = new T();
			if( result->Load( filename ) )
			{
				SDL_LockMutex( m_sdlMutex );
				m_mapAssets.insert( pair<string,Asset*>( filename, result ) );
				SDL_UnlockMutex( m_sdlMutex );
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
	Assets( const Assets& ){}

	map<string,Asset*> m_mapAssets;
	SDL_mutex* m_sdlMutex;
};

#endif