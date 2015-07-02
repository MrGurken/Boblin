#include "Script.h"

Script::Script( lua_State* lua )
	: m_pLua( lua ), m_bValid( false )
{
}

Script::~Script()
{
}

bool Script::Hotload()
{
	bool result = false;

	if( m_bValid )
	{
		FileInfo curInfo( m_strFilename );
		if( curInfo != m_fileInfo )
		{
			printf( "Script.cpp: Hotloading.\n" );
			Run( m_strFilename );

			result = true;
		}
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
		m_bValid = false;
	}
	else
	{
		m_strFilename = filename;
		m_bValid = true;
	}

	m_fileInfo.Get( filename );

	return result;
}

lua_State* Script::GetState() const { return m_pLua; }

// *******************************************************************
// Runtime
// *******************************************************************

Runtime& Runtime::Instance()
{
	static Runtime instance;
	return instance;
}

Runtime::Runtime()
{
	m_pLua = luaL_newstate();

	luaL_openlibs( m_pLua );
	File::lua_Register( m_pLua );
	GameObject::lua_Register( m_pLua );
	Texture::lua_Register( m_pLua );
	Camera::lua_Register( m_pLua );
	Maths::lua_Register( m_pLua );
}

Runtime::~Runtime()
{
	lua_close( m_pLua );
}

Runtime::operator lua_State*() const
{
	return m_pLua;
}

bool Runtime::Hotload()
{
	for( script_it it = m_vecScripts.begin(); it != m_vecScripts.end(); it++ )
		it->Hotload();
}

bool Runtime::Run( const string& filename )
{
	m_vecScripts.push_back( Script( m_pLua ) );
	script_rit it = m_vecScripts.rbegin();
	it->Run( filename );
}

void Runtime::Update()
{
	for( ref_it it = m_mapRefs.begin(); it != m_mapRefs.end(); it++ )
	{
		lua_rawgeti( m_pLua, LUA_REGISTRYINDEX, it->second );
		if( lua_pcall( m_pLua, 0, 0, 0 ) )
		{
			printf( "Script.cpp: Lua error: %s\n", lua_tostring( m_pLua, -1 ) );

			luaL_unref( m_pLua, LUA_REGISTRYINDEX, it->second );
			it = m_mapRefs.erase( it );
		}
	}
}

bool Runtime::Refer( const string& func )
{
	lua_getglobal( m_pLua, func.c_str() );
	if( lua_isfunction( m_pLua, -1 ) )
	{
		m_mapRefs.insert( pair<string,int>( func, luaL_ref( m_pLua, LUA_REGISTRYINDEX ) ) );
	}
	else
		lua_pop( m_pLua, 1 );
}

bool Runtime::UnRefer( const string& func )
{
	ref_it it = m_mapRefs.find( func );
	if( it != m_mapRefs.end() )
	{
		luaL_unref( m_pLua, LUA_REGISTRYINDEX, it->second );
		m_mapRefs.erase( it );
	}
}

void Runtime::lua_Register( lua_State* lua )
{
	lua_register( lua, "Run", lua_Run );
	lua_register( lua, "Refer", lua_Refer );
	lua_register( lua, "UnRefer", lua_UnRefer );
}

int Runtime::lua_Run( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		const char* script = lua_tostring( lua, 1 );
		lua_pushboolean( lua, Runtime::Instance().Run( script ) );
		result = 1;
	}

	return result;
}

int Runtime::lua_Refer( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		const char* func = lua_tostring( lua, 1 );
		lua_pushboolean( lua, Runtime::Instance().Refer( func ) );
		result = 1;
	}

	return result;
}

int Runtime::lua_UnRefer( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		const char* func = lua_tostring( lua, 1 );
		lua_pushboolean( lua, Runtime::Instance().UnRefer( func ) );
		result = 1;
	}

	return result;
}