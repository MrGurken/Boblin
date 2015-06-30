#ifndef FILE_H
#define FILE_H

#ifdef WIN32
#include <Windows.h>
#else
#include <dirent.h>
#endif

#include "lua.hpp"
#include <string>
using std::string;

#define MODE_FILES 0
#define MODE_DIRECTORIES 1
#define MODE_ALL 2

// NOTE: A directory is considered a file
// TODO: Do we really need to wrap this into a namespace
namespace File
{
	void lua_Register( lua_State* lua );
	int lua_Write( lua_State* lua, const char* path, bool directory );
	int lua_GetFiles( lua_State* lua );
	int lua_GetDirectories( lua_State* lua );
	int lua_GetAll( lua_State* lua );
	int lua_Get( lua_State* lua, int mode = MODE_FILES );
};

#endif