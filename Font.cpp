#include "Font.h"

Font::Font()
{
}

Font::~Font()
{
	// reference count quad mesh to delete it when no font is using it?
}

bool Font::Load( const AssetInfo* _info )
{
	const FontInfo* info = static_cast<const FontInfo*>( _info );

	bool result = false;

	TTF_Font* font = TTF_OpenFont( info->filename, info->size );
	if( font )
	{
		for( int i=0; i<FONT_GLYPHS; i++ )
			if( Glyph::GenerateGlyph( &m_glyphs[i], font, (char)i ) )
				result = true;

		m_iSize = info->size;

		TTF_CloseFont( font );
	}

	return result;
}

void Font::RenderText( Shader* shader, const char* text, vec2 position )
{
	vec2 offset;

	shader->SetUniform( "UVMin", vec2( 0, 0 ) );
	shader->SetUniform( "UVLength", vec2( 1, 1 ) );

	int len = strlen( text );
	for( int i=0; i<len; i++ )
	{
		if( text[i] == '\n' )
		{
			offset.x = 0;
			offset.y += static_cast<float>( m_iSize );
		}
		else
		{
			int index = text[i];
		
			RenderGlyph( shader, m_glyphs[index], position+offset );
			offset.x += m_glyphs[index].GetWidth();
		}
	}
}

void Font::RenderText( Shader* shader, const string& text, vec2 position )
{
	vec2 offset;

	shader->SetUniform( "UVMin", vec2( 0, 0 ) );
	shader->SetUniform( "UVLength", vec2( 1, 1 ) );

	for( string::const_iterator it = text.begin(); it != text.end(); it++ )
	{
		if( *it == '\n' )
		{
			offset.x = 0;
			offset.y += static_cast<float>( m_iSize );
		}
		else
		{
			int index = *it;

			RenderGlyph( shader, m_glyphs[index], position+offset );
			offset.x += m_glyphs[index].GetWidth();
		}
	}
}

void Font::RenderGlyph( Shader* shader, const Glyph& glyph, vec2 position )
{
	glyph.Bind();

	mat4 world = translate( vec3( position.x, position.y, 0.0f ) ) *
					scale( vec3( glyph.GetWidth(), glyph.GetHeight(), 1.0 ) );
	shader->SetUniform( "ModelMatrix", world );

	Mesh::GetQuad()->Render();
}

vec2 Font::Size( const char* text )
{
	float maxWidth = 0.0f;
	float curWidth = 0.0f;
	int curHeight = m_iSize;

	int len = strlen( text );
	for( int i=0; i<len; i++ )
	{
		if( text[i] == '\n' )
		{
			curHeight += m_iSize;
			curWidth = 0.0f;
		}
		else
		{
			int index = text[i];
			curWidth += m_glyphs[index].GetWidth();
			if( curWidth > maxWidth )
				maxWidth = curWidth;
		}
	}

	return vec2( maxWidth, static_cast<float>( curHeight ) );
}

vec2 Font::Size( const string& text )
{
	float maxWidth = 0.0f;
	float curWidth = 0.0f;
	int curHeight = m_iSize;

	for( string::const_iterator it = text.begin(); it != text.end(); it++ )
	{
		if( *it == '\n' )
		{
			curHeight += m_iSize;
			curWidth = 0.0f;
		}
		else
		{
			int index = *it;
			curWidth += m_glyphs[index].GetWidth();
			if( curWidth > maxWidth )
				maxWidth = curWidth;
		}
	}

	return vec2( maxWidth, static_cast<float>( curHeight ) );
}

int Font::GetSize() const
{
	return m_iSize;
}

// **************************************************************************************************
// Lua
// **************************************************************************************************

void Font::lua_Register( lua_State* lua )
{
	luaL_Reg funcs[] =
	{
		{ "Load", lua_Load },
		{ NULL, NULL }
	};

	luaL_newmetatable( lua, "Font" );
	luaL_setfuncs( lua, funcs, 0 );
	lua_pushvalue( lua, -1 );
	lua_setfield( lua, -2, "__index" );
	lua_setglobal( lua, "Font" );
}

Font* Font::lua_Read( lua_State* lua, int index )
{
	Font* result = nullptr;
	if( lua_istable( lua, index ) )
	{
		lua_getfield( lua, index, "__self" );
		result = reinterpret_cast<Font*>( lua_touserdata( lua, -1 ) );
	}
	return result;
}

int Font::lua_Write( lua_State* lua, Font* font )
{
	lua_newtable( lua );
	lua_pushlightuserdata( lua, font );
	lua_setfield( lua, -2, "__self" );
	lua_pushnumber( lua, font->m_iSize );
	lua_setfield( lua, -2, "Size" );
	luaL_getmetatable( lua, "Font" );
	lua_setmetatable( lua, -2 );
	return 1;
}

int Font::lua_Load( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 2 )
	{
		FontInfo fontInfo;
		fontInfo.filename = lua_tostring( lua, 1 );
		fontInfo.size = static_cast<int>( lua_tonumber( lua, 2 ) );

		Font* ptr = Assets::Instance().Load<Font>( &fontInfo );
		if( ptr )
		{
			result = lua_Write( lua, ptr );
		}
	}

	return result;
}