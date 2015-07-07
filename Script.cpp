#include "Script.h"

Script::Script( lua_State* lua )
	: m_pLua( lua ), m_bValid( false )
{
}

Script::Script( const Script& ref )
	: m_pLua( ref.m_pLua ), m_bValid( ref.m_bValid ),
	m_strFilename( ref.m_strFilename ),	m_uFileTime( ref.m_uFileTime )
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
		uint64_t curtime = FileInfo::GetLastModified( m_strFilename );

		if( curtime != m_uFileTime )
		{
			printf( "Script.cpp: Hotloading = %s\n", m_strFilename.c_str() );
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

	m_uFileTime = FileInfo::GetLastModified( filename );

	return result;
}

const string& Script::GetFilename() const { return m_strFilename; }
uint64_t Script::GetFileTime() const { return m_uFileTime; }

// *******************************************************************
// Runtime
// *******************************************************************

Runtime& Runtime::Instance()
{
	static Runtime instance;
	return instance;
}

Runtime::Runtime()
	: m_bRunning( true )
{
	m_pLua = luaL_newstate();

	luaL_openlibs( m_pLua );
	lua_Register( m_pLua );
	File::lua_Register( m_pLua );
	GameObject::lua_Register( m_pLua );
	Texture::lua_Register( m_pLua );
	Camera::lua_Register( m_pLua );
	Maths::lua_Register( m_pLua );
	Input::lua_Register( m_pLua );
	Config::lua_Register( m_pLua );

	for( int i=0; i<RUNTIME_MAX_SCRIPTS; i++ )
		m_pScripts[i] = nullptr;
}

Runtime::~Runtime()
{
	for( int i=0; i<RUNTIME_MAX_SCRIPTS; i++ )
		delete m_pScripts[i];

	lua_close( m_pLua );
}

Runtime::operator lua_State *() const
{
	return m_pLua;
}

void Runtime::Quit()
{
	m_bRunning = false;
}

bool Runtime::Hotload()
{
	bool result = false;

	for( int i=0; i<RUNTIME_MAX_SCRIPTS; i++ )
		if( m_pScripts[i] )
			result = result || m_pScripts[i]->Hotload();

	return result;
}

bool Runtime::Run( const string& filename )
{
	bool result = false;
	bool found = false;

	for( int i=0; i<RUNTIME_MAX_SCRIPTS; i++ )
	{
		if( m_pScripts[i] && m_pScripts[i]->GetFilename().compare( filename ) == 0 )
		{
			uint64_t curtime = FileInfo::GetLastModified( filename );

			if( curtime != m_pScripts[i]->GetFileTime() )
			{
				result = m_pScripts[i]->Run( filename );
			}

			found = true;
		}
	}

	if( !found )
	{
		int index = -1;
		for( int i=0; i<RUNTIME_MAX_SCRIPTS && index < 0; i++ )
			if( !m_pScripts[i] )
				index = i;

		if( index >= 0 )
		{
			m_pScripts[index] = new Script( m_pLua );
			m_pScripts[index]->Run( filename );
		}
	}

	return result;
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
}

void Runtime::Unrefer( int ref )
{
	int index = FindRef( ref );
	if( index >= 0 )
	{
		luaL_unref( m_pLua, LUA_REGISTRYINDEX, ref );
		m_refs[index].valid = false;
	}
}

int Runtime::FindRef( int ref )
{
	int index = -1;
	for( int i=0; i<RUNTIME_MAX_REFS && index<0; i++ )
		if( m_refs[i].ref == ref && m_refs[i].valid )
			index = i;
	return index;
}

int Runtime::Seconds( int sec )
{
	return ( sec * Config::Instance().GetFPS() );
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

lua_State* Runtime::GetState() const
{
	return m_pLua;
}

bool Runtime::GetRunning() const
{
	return m_bRunning;
}

void Runtime::lua_Register( lua_State* lua )
{
	lua_register( lua, "Run", lua_Run );
	lua_register( lua, "Refer", lua_Refer );
	lua_register( lua, "Unrefer", lua_Unrefer );
	lua_register( lua, "Seconds", lua_Seconds );
	lua_register( lua, "Quit", lua_Quit );
}

int Runtime::lua_Run( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		const char* filename = lua_tostring( lua, 1 );
		lua_pushboolean( lua, Runtime::Instance().Run( filename ) );
		result = 1;
	}

	return result;
}

int Runtime::lua_Refer( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 && lua_isfunction( lua, 1 ) )
	{
		int ref = luaL_ref( lua, LUA_REGISTRYINDEX );
		Runtime::Instance().Refer( ref );
		lua_pushnumber( lua, ref );
		result = 1;
	}

	return result;
}

int Runtime::lua_Unrefer( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int ref = static_cast<int>( lua_tonumber( lua, 1 ) );
		Runtime::Instance().Unrefer( ref );
	}

	return result;
}

int Runtime::lua_Seconds( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int sec = static_cast<int>( lua_tonumber( lua, 1 ) );
		lua_pushnumber( lua, Runtime::Instance().Seconds( sec ) );
		result = 1;
	}

	return result;
}

int Runtime::lua_Quit( lua_State* lua )
{
	Runtime::Instance().Quit();
	return 0;
}