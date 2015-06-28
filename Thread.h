#ifndef THREAD_H
#define THREAD_H

#include "lua.hpp"
#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_mutex.h"
#include <string>
using std::string;
#include <vector>
using std::vector;

#define MAX_THREADS 5

class Thread
{
public:
	Thread();
	//Thread( const string& script, const string& func, bool autoclean );
	virtual ~Thread();

	static void Alloc();
	static void Dealloc();
	static void Clean();

	void		Create( const string& script, const string& func, bool autoclean );
	void		Start();
	int			Wait();
	bool		Done();

	static void	lua_Register( lua_State* lua );
	static Thread* lua_Read( lua_State* lua, int index );
	static int	lua_Write( lua_State* lua, Thread* thread );

	static int	lua_Create( lua_State* lua );
	static int	lua_Start( lua_State* lua );
	static int	lua_Wait( lua_State* lua );
	static int	lua_Done( lua_State* lua );
	static int	lua_Sleep( lua_State* lua );

private:
	//static vector<Thread*>::iterator FindThread( Thread* thread );
	static int	DoWork( void* data );

	Thread( const Thread& ref ){}
	Thread& operator=( const Thread& ref ) { return *this; }

	SDL_Thread*	m_sdlThread;
	string		m_strScript;
	string		m_strFunc;
	bool		m_bDone;
	bool		m_bAutoClean;
	bool		m_bUsed;

	//static vector<Thread*> s_vecThreads;
	static SDL_mutex* s_sdlMutex;
	static Thread s_threads[MAX_THREADS];
};

#endif