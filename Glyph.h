#ifndef GLYPH_H
#define GLYPH_H

#include "LibIncluder.h"

class Glyph
{
public:
	Glyph();
	virtual ~Glyph();

	static bool GenerateGlyph( Glyph* glyph, TTF_Font* font, char character );

	void Bind() const;

	GLuint GetID() const;
	char GetCharacter() const;
	int GetWidth() const;
	int GetHeight() const;

private:
	GLuint m_glID;
	char m_cCharacter;
	int m_iWidth;
	int m_iHeight;
};

#endif