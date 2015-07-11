#ifndef TEXT_H
#define TEXT_H

#include "lua.hpp"
#include "Font.h"
#include <string>
using std::string;
#include <vector>
using std::vector;

class Text
{
public:
	Text();
	Text( const Text& ref );
	~Text();

	Text& operator=( const Text& ref );

	void			Render( Shader* shader );

	static void		RenderAll( Shader* shader );

	void			SetText( const string& text );
	void			SetPosition( vec2 position );
	void			SetColor( vec4 color );
	void			SetVisible( bool visible );
	void			SetFont( Font* font );

	const string&	GetText() const;
	vec2			GetPosition() const;
	vec2			GetSize() const;
	rect			GetBounds() const;
	vec4			GetColor() const;
	bool			GetVisible() const;
	Font*			GetFont() const;

	static void		lua_Register( lua_State* lua );
	static Text*	lua_Read( lua_State* lua, int index );
	static int		lua_Write( lua_State* lua, Text* text );
	static int		lua_Create( lua_State* lua );
	static int		lua_Copy( lua_State* lua );
	static int		lua_Destroy( lua_State* lua );
	static int		lua_Text( lua_State* lua );
	static int		lua_Position( lua_State* lua );
	static int		lua_Size( lua_State* lua );
	static int		lua_Bounds( lua_State* lua );
	static int		lua_Color( lua_State* lua );
	static int		lua_Visible( lua_State* lua );
	static int		lua_Font( lua_State* lua );
	
private:
	string			m_strText;
	vec2			m_size;
	vec2			m_position;
	vec4			m_color;
	bool			m_bVisible;
	Font*			m_pFont;

	static vector<Text*> s_vecTexts;
};

#endif