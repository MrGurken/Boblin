#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef WIN32
#include "GL\glew.h"
#include "SDL.h"
#include "SDL_image.h"
#else
#include <OpenGL/gl3.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#endif

#include "lua.hpp"
#include "Assets.h"

class Texture : public Asset
{
public:
	Texture();
	Texture( const Texture& ref );
	virtual ~Texture();

	bool	Load( const string& filename ) override;
	void	Unload() override;

	void	Bind( GLenum slot = GL_TEXTURE0 );

	int		GetWidth() const;
	int		GetHeight() const;

	// lua
	static void lua_Register( lua_State* lua );
	static Texture* lua_Read( lua_State* lua, int index );
	static int lua_Write( lua_State* lua, Texture* texture );
	static int lua_Load( lua_State* lua );
	static int lua_Dimensions( lua_State* lua );

private:
	GLuint	m_glID;
	int		m_iWidth;
	int		m_iHeight;
};

#endif