#ifndef SCRIPT_H
#define SCRIPT_H

#include "lua.hpp"
#include <vector>
using std::vector;
#include <map>
using std::map;
using std::pair;

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
	Script( const Script& ref ){}
	Script& operator=( const Script& ref ){ return *this; }

	bool		m_bValid;
	string		m_strFilename;
	FileInfo	m_fileInfo;
	lua_State*	m_pLua;
};

class Runtime
{
public:
	static Runtime& Instance();
	virtual ~Runtime();

	operator	lua_State*() const;

	bool		Hotload();
	bool		Run( const string& filename );
	bool		Refer( const string& func );
	bool		UnRefer( const string& func );

	void		Update();

	lua_State*	GetState() const;

	static void	lua_Register( lua_State* lua );
	static int	lua_Run( lua_State* lua );
	static int	lua_Refer( lua_State* lua );
	static int	lua_UnRefer( lua_State* lua );

private:
	Runtime();
	Runtime( const Runtime& ref );
	Runtime& operator=( const Runtime& ref ){ return *this; }

	typedef map<string,int>::iterator ref_it;
	typedef vector<Script>::iterator script_it;
	typedef vector<Script>::reverse_iterator script_rit;

	lua_State*		m_pLua;
	map<string,int>	m_mapRefs;
	vector<Script>	m_vecScripts;
};

#endif