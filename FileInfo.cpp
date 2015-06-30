#include "FileInfo.h"

FileInfo::FileInfo()
	: m_modifiedTime( 0 )
{
}

FileInfo::FileInfo( const string& filename )
{
	Get( filename );
}

FileInfo::FileInfo( const FileInfo& ref )
	: m_modifiedTime( ref.m_modifiedTime )
{
}

FileInfo::~FileInfo()
{
}

FileInfo& FileInfo::operator=( const FileInfo& ref )
{
	m_modifiedTime = ref.m_modifiedTime;
	return *this;
}

bool FileInfo::operator==( const FileInfo& ref )
{
	return ( m_modifiedTime == ref.m_modifiedTime );
}

bool FileInfo::operator!=( const FileInfo& ref )
{
	return !operator==( ref );
}

void FileInfo::Get( const string& filename )
{
#ifdef WIN32
	WIN32_FIND_DATA findData;
	HANDLE file = FindFirstFileA( filename.c_str(), &findData );
	if( file )
	{
		m_modifiedTime = FILETIME_TO_UINT64( findData.ftLastWriteTime );
		FindClose( file );
	}
#else
	struct stat data;
	if( stat( filename.c_str(), &data ) >= 0 )
	{
		m_modifiedTime = data.st_mtime;
	}
#endif
}

uint64_t FileInfo::GetModifiedTime() const { return m_modifiedTime; }