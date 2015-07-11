#ifndef SCRIPT_H
#define SCRIPT_H

#include "lua.hpp"
#include <vector>
using std::vector;

#include "maths.h"
#include "File.h"
#include "GameObject.h"
#include "Text.h"
#include "Texture.h"
#include "FileInfo.h"
#include "Camera.h"
#include "Input.h"
#include "Config.h"

class Script
{
public:
	Script( lua_State* lua );
	Script( const Script& ref );
	virtual ~Script();

	bool			Hotload();
	bool			Run( const string& filename );

	const string&	GetFilename() const;
	//const FileInfo&	GetFileInfo() const;
	uint64_t		GetFileTime() const;

private:
	bool			m_bValid;
	string			m_strFilename;
	//FileInfo		m_fileInfo;
	uint64_t		m_uFileTime;
	lua_State*		m_pLua;
};

struct FuncRef
{
	FuncRef() : valid( false ){}

	int ref;
	int valid; // 32 bit alignment
};

#define RUNTIME_MAX_REFS 32
#define RUNTIME_MAX_SCRIPTS 64

class Runtime
{
public:
	static Runtime& Instance();
	virtual ~Runtime();

	operator lua_State*() const;

	void			Quit();
	bool			Hotload();
	bool			Run( const string& filename );
	void			Refer( int ref );
	void			Unrefer( int ref );
	int				Seconds( int sec );

	void			Update();

	lua_State*		GetState() const;
	bool			GetRunning() const;

	static void		lua_Register( lua_State* lua );
	static int		lua_Run( lua_State* lua );
	static int		lua_Refer( lua_State* lua );
	static int		lua_Unrefer( lua_State* lua );
	static int		lua_Seconds( lua_State* lua );
	static int		lua_ClearColor( lua_State* lua );
	static int		lua_Quit( lua_State* lua );

private:
	Runtime();
	Runtime( const Runtime& ref ){}
	Runtime& operator=( const Runtime& ref ) { return *this; }

	typedef vector<Script>::iterator script_it;
	typedef vector<Script>::reverse_iterator script_rit;

	int				FindRef( int ref );

	lua_State*		m_pLua;
	Script*			m_pScripts[RUNTIME_MAX_SCRIPTS];
	FuncRef			m_refs[RUNTIME_MAX_REFS];
	bool			m_bRunning;
};

#endif