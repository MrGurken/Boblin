#ifndef SCRIPT_H
#define SCRIPT_H

#include "lua.hpp"

#include "File.h"
#include "GameObject.h"
#include "Texture.h"
#include "FileInfo.h"
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

private:
	Script( const Script& ref ){}
	Script& operator=( const Script& ref ){ return *this; }

	string		m_strFilename;
	FileInfo	m_fileInfo;
	FileInfo	m_lastFileInfo;
	lua_State*	m_pLua;
};

#endif