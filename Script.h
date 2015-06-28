#ifndef SCRIPT_H
#define SCRIPT_H

#ifdef WIN32
#include <Windows.h>
#else
#endif

#include "lua.hpp"

#include "GameObject.h"
#include "Texture.h"
#include "FileInfo.h"
#include "Thread.h"
#include "Camera.h"

class Script
{
public:
	Script();
	virtual ~Script();

	operator	lua_State*() const;

	bool		Hotload();
	bool		Run( const string& filename );

	lua_State*	GetState() const;

	static int	lua_FilesInDirectory( lua_State* lua );

private:
	Script( const Script& ref ){}
	Script& operator=( const Script& ref ){ return *this; }

	string		m_strFilename;
	FileInfo	m_fileInfo;
	FileInfo	m_lastFileInfo;
	lua_State*	m_pLua;
};

#endif