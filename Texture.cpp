#include "Texture.h"

Texture::Texture()
	: m_glID( 0 ), m_iWidth( 0 ), m_iHeight( 0 )
{
}

Texture::Texture( const Texture& ref )
	: m_glID( ref.m_glID ), m_iWidth( ref.m_iWidth ), m_iHeight( ref.m_iHeight )
{
}

Texture::~Texture()
{
}

bool Texture::Load( const string& filename )
{
	bool result = false;

	SDL_Surface* img = IMG_Load( filename.c_str() );
	if( img )
	{
		glGenTextures( 1, &m_glID );
		glBindTexture( GL_TEXTURE_2D, m_glID );
		// TODO: Find a way to supply min and mag filter to this function
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

		GLenum mode = GL_RGB;
		if( img->format->BytesPerPixel == 4 )
			mode = GL_RGBA;

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, mode, GL_UNSIGNED_BYTE, img->pixels );

		m_iWidth = img->w;
		m_iHeight = img->h;

		SDL_FreeSurface( img );
		result = true;
	}

	return result;
}

void Texture::Unload()
{
	if( m_glID != 0 )
		glDeleteTextures( 1, &m_glID );

	m_glID = 0;
	m_iWidth = m_iHeight = 0;
}

void Texture::Bind( GLenum slot )
{
	glActiveTexture( slot );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_glID );
}