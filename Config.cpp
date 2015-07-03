#include "Config.h"

Config& Config::Instance()
{
	static Config instance;
	return instance;
}

bool Config::Load( const string& filename )
{
	bool result = false;

	ifstream stream( filename );
	if( stream.is_open() )
	{
		stream >> m_iStartX;
		stream >> m_iStartY;
		stream >> m_iWidth;
		stream >> m_iHeight;

		getline( stream, m_strTitle );
		getline( stream, m_strScriptFolder );

		stream.close();
		result = true;
	}

	return result;
}

int Config::GetStartX() const { return m_iStartX; }
int Config::GetStartY() const { return m_iStartY; }
int Config::GetWidth() const { return m_iWidth; }
int Config::GetHeight() const { return m_iHeight; }
const string& Config::GetTitle() const { return m_strTitle; }
const string& Config::GetScriptFolder() const { return m_strScriptFolder; }