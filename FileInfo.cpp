#include "FileInfo.h"

uint64_t FileInfo::GetLastModified( const string& filename )
{
	uint64_t result = 0;

#ifdef WIN32
	WIN32_FIND_DATA findData;
	HANDLE file = FindFirstFileA( filename.c_str(), &findData );
	if( file )
	{
		result = FILETIME_TO_UINT64( findData.ftLastWriteTime );
		FindClose( file );
	}
#else
	struct stat datA;
	if( stat( filename.c_str(), &data ) >= 0 )
	{
		result = data.st_mtime;
	}
#endif

	return result;
}