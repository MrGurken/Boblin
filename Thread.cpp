#include "Thread.h"

vector<Thread*> Thread::s_vecThreads;
SDL_mutex* Thread::s_sdlMutex = NULL;

Thread::Thread( const string& script, const string& func, bool autoclean )
	: m_strScript( script ), m_strFunc( func ), m_bAutoClean( autoclean )
{
}

Thread::~Thread()
{
	printf( "Thread.cpp: Destroying thread.\n" );
}

void Thread::Alloc()
{
	s_sdlMutex = SDL_CreateMutex();
}

void Thread::Dealloc()
{
	SDL_DestroyMutex( s_sdlMutex );
}

void Thread::Clean()
{
	SDL_LockMutex( s_sdlMutex );
	for( vector<Thread*>::iterator it = s_vecThreads.begin(); it != s_vecThreads.end(); it++ )
	{
		if( (*it)->m_bAutoClean && (*it)->m_bDone )
		{
			(*it)->Wait();
			delete (*it);
			s_vecThreads.erase( it );
			break;
		}
	}
	SDL_UnlockMutex( s_sdlMutex );
}

void Thread::Start()
{
	m_sdlThread = SDL_CreateThread( DoWork, "LuaThread", this );
}

int Thread::Wait()
{
	int status;
	SDL_WaitThread( m_sdlThread, &status );

	return status;
}

bool Thread::Done()
{
	return m_bDone;
}

void Thread::lua_Register( lua_State* lua )
{
	luaL_Reg funcs[] =
	{
		{ "Create", lua_Create },
		{ "Start", lua_Start },
		{ "Wait", lua_Wait },
		{ "Done", lua_Done },
		{ "Sleep", lua_Sleep },
		{ NULL, NULL }
	};

	luaL_newmetatable( lua, "Thread" );
	luaL_setfuncs( lua, funcs, 0 );
	lua_pushvalue( lua, -1 );
	lua_setfield( lua, -1, "__index" );
	lua_setglobal( lua, "Thread" );
}

int Thread::lua_Create( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 2 )
	{
		bool autoclean = true;
		if( lua_gettop( lua ) >= 3 )
			autoclean = lua_toboolean( lua, 3 );

		Thread* t = new Thread( lua_tostring( lua, 1 ), lua_tostring( lua, 2 ), autoclean );

		// Add thread to global vector of threads
		SDL_LockMutex( s_sdlMutex );
		s_vecThreads.push_back( t );
		SDL_UnlockMutex( s_sdlMutex );

		lua_newtable( lua );
		lua_pushlightuserdata( lua, t );
		lua_setfield( lua, -2, "__self" );
		luaL_getmetatable( lua, "Thread" );
		lua_setmetatable( lua, -2 );

		result = 1;
	}

	return result;
}

int Thread::lua_Start( lua_State* lua )
{
	if( lua_gettop( lua ) >= 1 )
	{
		lua_getfield( lua, 1, "__self" );
		Thread* t = reinterpret_cast<Thread*>( lua_touserdata( lua, -1 ) );
		lua_pop( lua, 1 );
		t->Start();
	}

	return 0;
}

int Thread::lua_Wait( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		lua_getfield( lua, 1, "__self" );
		Thread* t = reinterpret_cast<Thread*>( lua_touserdata( lua, -1 ) );
		lua_pop( lua, 1 );
		lua_pushnumber( lua, t->Wait() );
		result = 1;

		// Remove thread from global vector of threads
		SDL_LockMutex( s_sdlMutex );
		vector<Thread*>::iterator it = FindThread( t );
		if( it != s_vecThreads.end() )
			s_vecThreads.erase( it );
		SDL_UnlockMutex( s_sdlMutex );

		delete t;
	}

	return result;
}

int Thread::lua_Done( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		lua_getfield( lua, 1, "__self" );
		Thread* t = reinterpret_cast<Thread*>( lua_touserdata( lua, -1 ) );
		lua_pop( lua, 1 );

		lua_pushboolean( lua, t->Done() );
		result = 1;
	}

	return result;
}

int Thread::lua_Sleep( lua_State* lua )
{
	if( lua_gettop( lua ) >= 1 )
	{
		int ms = lua_tonumber( lua, 1 );
		SDL_Delay( ms );
	}
	return 0;
}

vector<Thread*>::iterator Thread::FindThread( Thread* thread )
{
	for( vector<Thread*>::iterator it = s_vecThreads.begin(); it != s_vecThreads.end(); it++ )
		if( *it == thread )
			return it;
	return s_vecThreads.end();
}

int Thread::DoWork( void* data )
{
	int result = 0;

	Thread* t = reinterpret_cast<Thread*>( data );
	t->m_bDone = false;

	lua_State* lua = luaL_newstate();
	luaL_openlibs( lua );
	lua_Register( lua );

	luaL_loadfile( lua, t->m_strScript.c_str() );
	if( lua_pcall( lua, 0, 0, 0 ) )
	{
		printf( "Thread.cpp: Lua error: %s\n", lua_tostring( lua, -1 ) );
		result = -1;
	}
	else
	{
		lua_getglobal( lua, t->m_strFunc.c_str() );
		if( lua_pcall( lua, 0, 0, 0 ) )
		{
			printf( "Thread.cpp: Lua error: %s\n", lua_tostring( lua, -1 ) );
			result = -1;
		}
	}

	lua_close( lua );
	t->m_bDone = true;

	return result;
}