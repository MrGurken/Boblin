#include "FileInfo.h"

FileInfo::FileInfo()
	: m_creationTime( 0 ), m_modifiedTime( 0 ), m_accessTime( 0 )
{
}

FileInfo::FileInfo( const string& filename )
{
	Get( filename );
}

FileInfo::FileInfo( const FileInfo& ref )
	: m_creationTime( ref.m_creationTime ), m_modifiedTime( ref.m_modifiedTime ), m_accessTime( ref.m_accessTime )
{
}

FileInfo::~FileInfo()
{
}

FileInfo& FileInfo::operator=( const FileInfo& ref )
{
	m_creationTime = ref.m_creationTime;
	m_modifiedTime = ref.m_modifiedTime;
	m_accessTime = ref.m_accessTime;
}

bool FileInfo::operator==( const FileInfo& ref )
{
	return ( m_creationTime == ref.m_creationTime &&
			 m_modifiedTime == ref.m_modifiedTime &&
			 m_accessTime == ref.m_accessTime );
}

bool FileInfo::operator!=( const FileInfo& ref )
{
	return !operator==( ref );
}

void FileInfo::Get( const string& filename )
{
#if WIN32
	WIN32_FIND_DATA findData;
	HANDLE file = FindFirstFileA( filename.c_str(), &findData );
	if( file )
	{
		FILETIME creation, modified, access;
		if( GetFileTime( file, &creation, &access, &modified ) )
		{
			m_creationTime = FILETIME_TO_UINT64( creation );
			m_modifiedTime = FILETIME_TO_UINT64( modified );
			m_accessTime = FILETIME_TO_UINT64( access );
		}
		FindClose( file );
	}
#else
#endif
}

uint64_t FileInfo::GetCreationTime() const { return m_creationTime; }
uint64_t FileInfo::GetModifiedTime() const { return m_modifiedTime; }
uint64_t FileInfo::GetAccessTime() const { return m_accessTime; }