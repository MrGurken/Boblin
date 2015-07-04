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
#include "Input.h"

class Script
{
public:
	Script( lua_State* lua );
	Script( const Script& ref );
	virtual ~Script();

	bool			Hotload();
	bool			Run( const string& filename );

	const string&	GetFilename() const;
	const FileInfo&	GetFileInfo() const;

private:
	bool			m_bValid;
	string			m_strFilename;
	FileInfo		m_fileInfo;
	lua_State*		m_pLua;
};

struct FuncRef
{
	FuncRef() : valid( false ){}

	int ref;
	int valid; // 32 bit alignment
};

#define RUNTIME_MAX_REFS 32

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

	void			Update();

	lua_State*		GetState() const;
	bool			GetRunning() const;

	static void		lua_Register( lua_State* lua );
	static int		lua_Run( lua_State* lua );
	static int		lua_Refer( lua_State* lua );
	static int		lua_Unrefer( lua_State* lua );
	static int		lua_Quit( lua_State* lua );

private:
	Runtime();
	Runtime( const Runtime& ref ){}
	Runtime& operator=( const Runtime& ref ) { return *this; }

	typedef vector<Script>::iterator script_it;
	typedef vector<Script>::reverse_iterator script_rit;

	int				FindRef( int ref );

	lua_State*		m_pLua;
	vector<Script>	m_vecScripts;
	FuncRef			m_refs[RUNTIME_MAX_REFS];
	bool			m_bRunning;
};

#endif