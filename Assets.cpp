#include "Assets.h"

Assets& Assets::Instance()
{
	static Assets instance;
	return instance;
}

void Assets::Unload()
{
	SDL_LockMutex( m_sdlMutex );
	for( map<string,Asset*>::iterator it = m_mapAssets.begin(); it != m_mapAssets.end(); it++ )
		delete it->second;
	m_mapAssets.clear();
	SDL_UnlockMutex( m_sdlMutex );
}