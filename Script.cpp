#include "Script.h"

Script::Script()
{
	m_pLua = luaL_newstate();

	luaL_openlibs( m_pLua );
	GameObject::lua_Register( m_pLua );
	Texture::lua_Register( m_pLua );
}

Script::~Script()
{
	lua_close( m_pLua );
}

Script::operator lua_State*() const
{
	return m_pLua;
}

bool Script::Hotload()
{
	bool result = false;

	FileInfo curInfo( m_strFilename );
	if( curInfo != m_fileInfo )
	{
		if( curInfo != m_lastFileInfo )
		{
			printf( "Script.cpp: Hotloading.\n" );
			Run( m_strFilename );

			result = true;
		}

		m_lastFileInfo = curInfo;
	}

	return result;
}

bool Script::Run( const string& filename )
{
	bool result = true;

	luaL_loadfile( m_pLua, filename.c_str() );
	if( lua_pcall( m_pLua, 0, 0, 0 ) )
	{
		printf( "Script.cpp: Lua error: %s\n", lua_tostring( m_pLua, -1 ) );
		result = false;
	}
	else
	{
		m_strFilename = filename;
		m_fileInfo.Get( filename );
	}

	return result;
}

lua_State* Script::GetState() const { return m_pLua; }