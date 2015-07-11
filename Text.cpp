#include "Text.h"

vector<Text*> Text::s_vecTexts;

Text::Text()
	: m_color( 1.0f ), m_bVisible( false ), m_pFont( nullptr )
{
}

Text::Text( const Text& ref )
	: m_strText( ref.m_strText ), m_size( ref.m_size ),
	m_position( ref.m_position ), m_color( ref.m_color ),
	m_bVisible( ref.m_bVisible ), m_pFont( ref.m_pFont )
{
}

Text::~Text()
{
}

Text& Text::operator=( const Text& ref )
{
	m_strText = ref.m_strText;
	m_size = ref.m_size;
	m_position = ref.m_position;
	m_color = ref.m_color;
	m_bVisible = ref.m_bVisible;
	m_pFont = ref.m_pFont;
	return *this;
}

void Text::Render( Shader* shader )
{
	if( m_pFont )
	{
		shader->SetUniform( "Color", m_color );

		m_pFont->RenderText( shader, m_strText, m_position );
	}
}

void Text::RenderAll( Shader* shader )
{
	for( vector<Text*>::iterator it = s_vecTexts.begin(); it != s_vecTexts.end(); it++ )
		if( (*it)->GetVisible() )
			(*it)->Render( shader );
}

void Text::SetText( const string& text )
{
	m_strText = text;
	if( m_pFont )
		m_size = m_pFont->Size( m_strText );
}
void Text::SetPosition( vec2 position )
{
	// avoid subpixels which makes text look blurry
	m_position.x = static_cast<float>( static_cast<int>( position.x ) );
	m_position.y = static_cast<float>( static_cast<int>( position.y ) );
}
void Text::SetColor( vec4 color ) { m_color = color; }
void Text::SetVisible( bool visible ) { m_bVisible = visible; }
void Text::SetFont( Font* font )
{
	m_pFont = font;
	if( m_pFont )
		m_size = m_pFont->Size( m_strText );
}

const string& Text::GetText() const { return m_strText; }
rect Text::GetBounds() const { return rect( m_position.x, m_position.y, m_size.x, m_size.y ); }
vec2 Text::GetSize() const { return m_size; }
vec2 Text::GetPosition() const { return m_position; }
vec4 Text::GetColor() const { return m_color; }
bool Text::GetVisible() const { return m_bVisible; }
Font* Text::GetFont() const { return m_pFont; }

// ******************************************************************************************************
// Lua
// ******************************************************************************************************

void Text::lua_Register( lua_State* lua )
{
	luaL_Reg funcs[] =
	{
		{ "Create", lua_Create },
		{ "Copy", lua_Copy },
		{ "Destroy", lua_Destroy },
		{ "Text", lua_Text },
		{ "Position", lua_Position },
		{ "Size", lua_Size },
		{ "Bounds", lua_Bounds },
		{ "Color", lua_Color },
		{ "Visible", lua_Visible },
		{ "Font", lua_Font },
		{ NULL, NULL }
	};

	luaL_newmetatable( lua, "Text" );
	luaL_setfuncs( lua, funcs, 0 );
	lua_pushvalue( lua, -1 );
	lua_setfield( lua, -2, "__index" );
	lua_setglobal( lua, "Text" );
}

Text* Text::lua_Read( lua_State* lua, int index )
{
	Text* result = nullptr;
	if( lua_istable( lua, index ) )
	{
		lua_getfield( lua, index, "__self" );
		result = reinterpret_cast<Text*>( lua_touserdata( lua, -1 ) );
	}

	return result;
}

int Text::lua_Write( lua_State* lua, Text* text )
{
	lua_newtable( lua );
	lua_pushlightuserdata( lua, text );
	lua_setfield( lua, -2, "__self" );
	luaL_getmetatable( lua, "Text" );
	lua_setmetatable( lua, -2 );
	return 1;
}

int Text::lua_Create( lua_State* lua )
{
	Text* ptr = new Text();
	s_vecTexts.push_back( ptr );
	return lua_Write( lua, ptr );
}

int Text::lua_Copy( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Text* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			Text* newptr = new Text( *ptr );
			s_vecTexts.push_back( newptr );
			result = lua_Write( lua, newptr );
		}
	}

	return result;
}

int Text::lua_Destroy( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Text* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			vector<Text*>::iterator it;
			for( it = s_vecTexts.begin(); it != s_vecTexts.end(); it++ )
				if( *it == ptr )
					break;

			s_vecTexts.erase( it );
			delete ptr;
		}
	}

	return result;
}

int Text::lua_Text( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Text* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 2 ) // setting
			{
				const char* text = lua_tostring( lua, 2 );
				ptr->SetText( text );
			}
			else // getting
			{
				lua_pushstring( lua, ptr->GetText().c_str() );
				result = 1;
			}
		}
	}

	return result;
}

int Text::lua_Position( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Text* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 3 ) // setting
				ptr->SetPosition( Vec2::lua_Parse( lua, 2 ) );
			else if( nargs >= 2 ) // setting
				ptr->SetPosition( Vec2::lua_Read( lua, 2 ) );
			else // getting
				result = Vec2::lua_Write( lua, ptr->GetPosition() );
		}
	}

	return result;
}

int Text::lua_Size( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Text* ptr = lua_Read( lua, 1 );
		if( ptr )
			result = Vec2::lua_Write( lua, ptr->GetSize() );
	}

	return result;
}

int Text::lua_Bounds( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Text* ptr = lua_Read( lua, 1 );
		if( ptr )
			result = rect::lua_Write( lua, ptr->GetBounds() );
	}

	return result;
}

int Text::lua_Color( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Text* ptr = lua_Read( lua, 1 );
		if( ptr )	
		{
			if( nargs >= 5 ) // setting
				ptr->SetColor( Vec4::lua_Parse( lua, 2 ) );
			else if( nargs >= 2 ) // setting
				ptr->SetColor( Vec4::lua_Read( lua, 2 ) );
			else // getting
				result = Vec4::lua_Write( lua, ptr->GetColor() );
		}
	}

	return result;
}

int Text::lua_Visible( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Text* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 2 ) // setting
				ptr->SetVisible( ( lua_toboolean( lua, 2 ) != 0 ) );
			else // getting
			{
				lua_pushboolean( lua, ptr->GetVisible() );
				result = 1;
			}
		}
	}

	return result;
}

int Text::lua_Font( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Text* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 2 ) // setting
				ptr->SetFont( Font::lua_Read( lua, 2 ) );
			else // getting
				result = Font::lua_Write( lua, ptr->GetFont() );
		}
	}

	return result;
}