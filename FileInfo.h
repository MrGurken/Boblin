#ifndef FILE_INFO_H
#define FILE_INFO_H

#ifdef WIN32
#include <Windows.h>
#define FILETIME_TO_UINT64( val ) ( ((uint64_t)val.dwHighDateTime << 32) | ((uint64_t)val.dwLowDateTime ) )
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <stdint.h>
#include <string>
using std::string;

class FileInfo
{
public:
	FileInfo();
	FileInfo( const string& filename );
	FileInfo( const FileInfo& ref );
	virtual ~FileInfo();

	FileInfo&	operator=( const FileInfo& ref );
	bool		operator==( const FileInfo& ref );
	bool		operator!=( const FileInfo& ref );

	void		Get( const string& filename );

	uint64_t	GetModifiedTime() const;

private:
	uint64_t	m_modifiedTime;
};

#endif