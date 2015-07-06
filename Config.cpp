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
		stream >> m_iFPS;
		stream.ignore();

		getline( stream, m_strTitle );
		getline( stream, m_strScriptFolder );

		stream.close();
		result = true;
	}

	return result;
}

int Config::GetFPS() const { return m_iFPS; }
int Config::GetStartX() const { return m_iStartX; }
int Config::GetStartY() const { return m_iStartY; }
int Config::GetWidth() const { return m_iWidth; }
int Config::GetHeight() const { return m_iHeight; }
const string& Config::GetTitle() const { return m_strTitle; }
const string& Config::GetScriptFolder() const { return m_strScriptFolder; }

void Config::lua_Register( lua_State* lua )
{
	lua_newtable( lua );
	lua_pushnumber( lua, Config::Instance().m_iStartX );
	lua_setfield( lua, -2, "StartX" );
	lua_pushnumber( lua, Config::Instance().m_iStartY );
	lua_setfield( lua, -2, "StartY" );
	lua_pushnumber( lua, Config::Instance().m_iWidth );
	lua_setfield( lua, -2, "Width" );
	lua_pushnumber( lua, Config::Instance().m_iHeight );
	lua_setfield( lua, -2, "Height" );
	lua_pushnumber( lua, Config::Instance().m_iFPS );
	lua_setfield( lua, -2, "FPS" );
	lua_pushstring( lua, Config::Instance().m_strTitle.c_str() );
	lua_setfield( lua, -2, "Title" );
	lua_pushstring( lua, Config::Instance().m_strScriptFolder.c_str() );
	lua_setfield( lua, -2, "ScriptFolder" );
	lua_setglobal( lua, "Config" );
}