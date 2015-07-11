#include "Glyph.h"

Glyph::Glyph()
	: m_glID( 0 ), m_cCharacter( 0 ), m_iWidth( 0 ), m_iHeight( 0 )
{
}

Glyph::~Glyph()
{
	if( m_glID > 0 )
		glDeleteTextures( 1, &m_glID );
}

bool Glyph::GenerateGlyph( Glyph* glyph, TTF_Font* font, char character )
{
	bool result = false;

	SDL_Color color = { 1, 1, 1 };
	SDL_Surface* surface = TTF_RenderGlyph_Blended( font, character, color );
	if( surface )
	{
		glGenTextures( 1, &glyph->m_glID );
		glBindTexture( GL_TEXTURE_2D, glyph->m_glID );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		GLenum format = ( surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels );

		glyph->m_cCharacter = character;
		glyph->m_iWidth = surface->w;
		glyph->m_iHeight = surface->h;

		SDL_FreeSurface( surface );
		result = true;
	}

	return result;
}

void Glyph::Bind() const
{
	glBindTexture( GL_TEXTURE_2D, m_glID );
}

GLuint Glyph::GetID() const { return m_glID; }
char Glyph::GetCharacter() const { return m_cCharacter; }
int Glyph::GetWidth() const { return m_iWidth; }
int Glyph::GetHeight() const { return m_iHeight; }