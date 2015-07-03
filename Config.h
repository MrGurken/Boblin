#ifndef CONFIG_H
#define CONFIG_H

#include "lua.hpp"
#include <string>
using std::string;
#include <fstream>
using std::ifstream;

class Config
{
public:
	static Config& Instance();
	virtual ~Config(){}

	bool			Load( const string& filename );

	int				GetStartX() const;
	int				GetStartY() const;
	int				GetWidth() const;
	int				GetHeight() const;
	const string&	GetTitle() const;
	const string&	GetScriptFolder() const;

private:
	Config(){}
	Config( const Config& ref ){}
	Config& operator=( const Config& ref ) { return *this; }

	int				m_iStartX;
	int				m_iStartY;
	int				m_iWidth;
	int				m_iHeight;
	string			m_strTitle;
	string			m_strScriptFolder;
};

#endif