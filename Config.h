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
	int				GetFPS() const;
	const string&	GetTitle() const;
	const string&	GetScriptFolder() const;

	static void		lua_Register( lua_State* lua );

private:
	Config(){}
	Config( const Config& ref ){}
	Config& operator=( const Config& ref ) { return *this; }

	int				m_iStartX;
	int				m_iStartY;
	int				m_iWidth;
	int				m_iHeight;
	int				m_iFPS;
	string			m_strTitle;
	string			m_strScriptFolder;
};

#endif