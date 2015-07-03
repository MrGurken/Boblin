#ifndef SCRIPT_H
#define SCRIPT_H

#include "lua.hpp"
#include <vector>
using std::vector;

#include "maths.h"
#include "File.h"
#include "GameObject.h"
#include "Texture.h"
#include "FileInfo.h"
#include "Camera.h"

class Script
{
public:
	Script( lua_State* lua );
	virtual ~Script();

	bool		Hotload();
	bool		Run( const string& filename );

	lua_State*	GetState() const;

private:
	bool		m_bValid;
	string		m_strFilename;
	FileInfo	m_fileInfo;
	lua_State*	m_pLua;
};

struct FuncRef
{
	FuncRef() : valid( false ){}

	int ref;
	int valid; // 32 bit alignment
};

#define RUNTIME_MAX_REFS 32
#define RUNTIME_REF_INDICES 2

class Runtime
{
public:
	static Runtime& Instance();
	virtual ~Runtime();

	operator lua_State*() const;

	bool			Hotload();
	bool			Run( const string& filename );
	void			Refer( int ref, int update );
	void			Unrefer( int ref );

	void			Update();
	void			Render();

	lua_State*		GetState() const;

	static void		lua_Register( lua_State* lua );
	static int		lua_Run( lua_State* lua );
	static int		lua_ReferUpdate( lua_State* lua );
	static int		lua_ReferRender( lua_State* lua );
	static int		lua_Unrefer( lua_State* lua );

private:
	Runtime();
	Runtime( const Runtime& ref ){}
	Runtime& operator=( const Runtime& ref ) { return *this; }

	typedef vector<Script>::iterator script_it;
	typedef vector<Script>::reverse_iterator script_rit;

	int				FindRef( FuncRef refs[RUNTIME_MAX_REFS], int ref );

	lua_State*		m_pLua;
	vector<Script>	m_vecScripts;
	FuncRef			m_updateRefs[RUNTIME_MAX_REFS];
	FuncRef			m_renderRefs[RUNTIME_MAX_REFS];
};

#endif