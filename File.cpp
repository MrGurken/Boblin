#include "File.h"

void File::lua_Register( lua_State* lua )
{
	lua_register( lua, "GetFiles", lua_GetFiles );
	lua_register( lua, "GetDirectories", lua_GetDirectories );
	lua_register( lua, "GetAll", lua_GetAll );
}

int File::lua_Write( lua_State* lua, const char* path, bool directory )
{
	lua_newtable( lua );
	lua_pushstring( lua, path );
	lua_setfield( lua, -2, "path" );
	lua_pushboolean( lua, directory);
	lua_setfield( lua, -2, "directory" );
	return 1;
}

int File::lua_GetFiles( lua_State* lua )
{
	return lua_Get( lua, MODE_FILES );
}

int File::lua_GetDirectories( lua_State* lua )
{
	return lua_Get( lua, MODE_DIRECTORIES );
}

int File::lua_GetAll( lua_State* lua )
{
	return lua_Get( lua, MODE_ALL );
}

int File::lua_Get( lua_State* lua, int mode )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		const char* path = lua_tostring( lua, 1 );

#ifdef WIN32
		WIN32_FIND_DATA findData;
		HANDLE file = FindFirstFile( path, &findData );
		if( file != INVALID_HANDLE_VALUE )
		{
			int curFile = 1;
			lua_newtable( lua );

			do
			{
				const char* path = findData.cFileName;
				int directory = ( ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) ? 1 : 0 );

				if( mode == MODE_ALL || ( mode == directory ) )
				{
					if( !directory || ( strcmp( ".", path ) != 0 && strcmp( "..", path ) != 0 ) )
					{
						lua_Write( lua, path, ( directory == 1 ) );
						lua_rawseti( lua, -2, curFile++ );
					}
				}
			} while( FindNextFile( file, &findData ) != 0 );

			result = 1;
		}
#else
		DIR* dir = opendir( path );
		if( dir )
		{
			int curFile = 1;
			lua_newtable( lua );

			struct dirent* ent;
			while( ( ent = readdir( dir ) ) )
			{
				const char* path = ent->d_name;
				int directory = ( ( ( ent->d_type & DT_DIR ) != 0 ) ? 1 : 0 );

				if( mode == MODE_ALL || ( mode == directory ) )
				{
					if( !directory || ( strcmp( ".", path ) != 0 && strcmp( "..", path ) != 0 ) )
					{
						lua_Write( lua, path, ( directory == 1 ) );
						lua_rawseti( lua, -2, curFile++ );
					}
				}
			}

			result = 1;
		}
#endif
	}

	return result;
}