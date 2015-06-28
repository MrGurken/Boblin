#include "Script.h"

Script::Script()
{
	m_pLua = luaL_newstate();

	luaL_openlibs( m_pLua );
	GameObject::lua_Register( m_pLua );
	Texture::lua_Register( m_pLua );
	Thread::lua_Register( m_pLua );
	Camera::lua_Register( m_pLua );

	lua_register( m_pLua, "FilesInDirectory", lua_FilesInDirectory );
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

int Script::lua_FilesInDirectory( lua_State* lua )
{
	int result = 0;
	if( lua_gettop( lua ) >= 1 )
	{
		const char* dir = lua_tostring( lua, 1 );

		int curFile = 1;
		lua_newtable( lua );
#ifdef WIN32
		WIN32_FIND_DATA findData;
		HANDLE file = FindFirstFile( dir, &findData );
		if( file != INVALID_HANDLE_VALUE )
		{
			do
			{
				if( !( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
				{
					lua_pushstring( lua, findData.cFileName );
					lua_rawseti( lua, -2, curFile++ );
				}
			} while( FindNextFile( file, &findData ) != 0 );
			FindClose( file );
		}
#else
	// TODO: Implement POSIX version
#endif
		result = 1;
	}
	return result;
}