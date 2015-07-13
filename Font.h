#ifndef FONT_H
#define FONT_H

#include "lua.hpp"
#include "glm.hpp"
#include "matrix_transform.hpp"
using namespace glm;

#include "LibIncluder.h"
#include "Assets.h"
#include "Glyph.h"
#include "mesh.h"
#include "Shader.h"

#define FONT_GLYPHS 128
#define FONT_ASCII_MIN 32
#define FONT_ASCII_MAX 125

struct FontInfo : public AssetInfo
{
	int size;
};

class Font : public Asset
{
public:
	Font();
	virtual ~Font();

	bool	Load( const AssetInfo* info ) override;

	void	RenderText( Shader* shader, const char* text, vec2 position );
	void	RenderText( Shader* shader, const string& text, vec2 position );
	void	RenderGlyph( Shader* shader, const Glyph& glyph, vec2 position );
	vec2	Size( const char* text );
	vec2	Size( const string& text );

	int		GetSize() const;

	static void	lua_Register( lua_State* lua );
	static Font* lua_Read( lua_State* lua, int index );
	static int lua_Write( lua_State* lua, Font* font );
	static int lua_Load( lua_State* lua );

private:
	Glyph	m_glyphs[FONT_GLYPHS];
	int		m_iSize;
};

#endif