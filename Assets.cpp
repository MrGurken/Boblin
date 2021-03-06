#include "Assets.h"

Asset::Asset(){}
Asset::~Asset(){}
//bool Asset::Load( const string& filename ) { return false; }
bool Asset::Load( const AssetInfo* info ) { return false; }
void Asset::Unload() {}

Assets& Assets::Instance()
{
	static Assets instance;
	return instance;
}

void Assets::Unload()
{
	for( map<string,Asset*>::iterator it = m_mapAssets.begin(); it != m_mapAssets.end(); it++ )
		delete it->second;
	m_mapAssets.clear();
}