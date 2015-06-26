#ifndef TEXTURE_H
#define TEXTURE_H

#include "Assets.h"
#include "GL\glew.h"
#include "SDL.h"
#include "SDL_image.h"

class Texture : public Asset
{
public:
	Texture();
	Texture( const Texture& ref );
	virtual ~Texture();

	bool	Load( const string& filename ) override;
	void	Unload() override;

	void	Bind( GLenum slot = GL_TEXTURE0 );

private:
	GLuint	m_glID;
	int		m_iWidth;
	int		m_iHeight;
};

#endif