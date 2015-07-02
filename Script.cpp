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
	lua_Register( m_pLua );
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
	bool result = false;

	for( script_it it = m_vecScripts.begin(); it != m_vecScripts.end(); it++ )
		result = ( result || it->Hotload() );

	return result;
}

bool Runtime::Run( const string& filename )
{
	m_vecScripts.push_back( Script( m_pLua ) );
	script_rit it = m_vecScripts.rbegin();
	return it->Run( filename );
}

void Runtime::Update()
{
	for( int i=0; i<RUNTIME_MAX_REFS; i++ )
	{
		if( m_refs[i].valid )
		{
			lua_rawgeti( m_pLua, LUA_REGISTRYINDEX, m_refs[i].ref );
			if( lua_pcall( m_pLua, 0, 0, 0 ) )
			{
				printf( "Script.cpp: Lua error: %s\n", lua_tostring( m_pLua, -1 ) );

				luaL_unref( m_pLua, LUA_REGISTRYINDEX, m_refs[i].ref );
				m_refs[i].valid = false;
			}
		}
	}
}

void Runtime::Refer( int ref )
{
	int index = FindRef( ref );
	if( index < 0 )
	{
		for( int i=0; i<RUNTIME_MAX_REFS; i++ )
		{
			if( !m_refs[i].valid )
			{
				m_refs[i].ref = ref;
				m_refs[i].valid = true;
				break;
			}
		}
	}
	else
		m_refs[index].valid = true; // we don't really care if it was previously valid or not
}

void Runtime::UnRefer( int ref )
{
	int index = FindRef( ref );
	if( index >= 0 )
	{
		m_refs[index].valid = false;
		luaL_unref( m_pLua, LUA_REGISTRYINDEX, ref );
	}
}

int Runtime::FindRef( int ref )
{
	int index = -1;
	for( int i=0; i<RUNTIME_MAX_REFS && index < 0; i++ )
		if( m_refs[i].ref == ref )
			index = i;
	return index;
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

	if( lua_isfunction( lua, -1 ) )
	{
		int ref = luaL_ref( lua, LUA_REGISTRYINDEX );
		Runtime::Instance().Refer( ref );
		lua_pushnumber( lua, ref );
		result = 1;
	}

	return result;
}

int Runtime::lua_UnRefer( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int ref = static_cast<int>( lua_tonumber( lua, -1 ) );
		Runtime::Instance().UnRefer( ref );
	}

	return result;
}