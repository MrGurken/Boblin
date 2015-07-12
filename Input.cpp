#include "Input.h"

Input& Input::Instance()
{
	static Input instance;
	return instance;
}

void Input::SetWindow( SDL_Window* window ) { m_sdlWindow = window; }

bool Input::Update()
{
	assert( m_sdlWindow != nullptr );

	bool result = true;

	m_vecRepeatKeys.clear();

	m_vecPrevKeys = m_vecKeys;

	for( int i=0; i<INPUT_MAX_BUTTONS; i++ )
		m_prevButtons[i] = m_buttons[i];

	SDL_Event e;
	while( SDL_PollEvent( &e ) )
	{
		if( e.type == SDL_QUIT )
			result = false;
		else if( e.type == SDL_KEYDOWN )
		{
			if( !e.key.repeat )
				AddKey( m_vecKeys, e.key.keysym.scancode );
			AddKey( m_vecRepeatKeys, e.key.keysym.scancode );
		}
		else if( e.type == SDL_KEYUP )
		{
			RemoveKey( m_vecKeys, e.key.keysym.scancode );
		}
		else if( e.type == SDL_TEXTINPUT )
		{
			m_ssTextInput << e.text.text;
		}
		else if( e.type == SDL_MOUSEMOTION )
		{
			vec2 prevPos = m_mousePosition;
			m_mousePosition.x = static_cast<float>( e.motion.x );
			m_mousePosition.y = static_cast<float>( e.motion.y );
			m_mouseDelta = ( m_mousePosition - prevPos );
		}
		else if( e.type == SDL_MOUSEWHEEL )
		{
			m_iMouseWheelDelta = e.wheel.y;
		}
		else if( e.type == SDL_MOUSEBUTTONDOWN )
		{
			m_buttons[e.button.button-1] = true;
		}
		else if( e.type == SDL_MOUSEBUTTONUP )
		{
			m_buttons[e.button.button-1] = false;
		}
	}

	return result;
}

void Input::StartTextInput()
{
	if( m_iTextInputs <= 0 )
	{
		SDL_StartTextInput();
		printf( "Input.cpp: Starting text input.\n" );
	}

	m_iTextInputs++;
}

void Input::StopTextInput()
{
	m_iTextInputs--;

	if( m_iTextInputs <= 0 )
	{
		SDL_StopTextInput();
		printf( "Input.cpp: Stopping text input.\n" );
	}
}

string Input::GetTextInput()
{
	string result = m_ssTextInput.str();
	m_ssTextInput.str("");
	return result;
}

bool Input::GetTextInputEnabled() const
{
	return ( m_iTextInputs > 0 );
}

int Input::FindKey( vector<SDL_Scancode>& vec, SDL_Scancode key )
{
	int result = -1;
	for( size_t i=0; i<vec.size() && result < 0; i++ )
		if( vec[i] == key )
			result = static_cast<int>(i);
	return result;
}

void Input::AddKey( vector<SDL_Scancode>& vec, SDL_Scancode key )
{
	int index = FindKey( vec, key );
	if( index < 0 )
		vec.push_back( key );
}

void Input::RemoveKey( vector<SDL_Scancode>& vec, SDL_Scancode key )
{
	int index = FindKey( vec, key );
	if( index >= 0 )
		vec.erase( vec.begin()  + index );
}

bool Input::KeyDown( SDL_Scancode key )
{
	return ( FindKey( m_vecKeys, key ) >= 0 );
}

bool Input::KeyUp( SDL_Scancode key )
{
	return ( FindKey( m_vecKeys, key ) < 0 );
}

bool Input::KeyPressed( SDL_Scancode key )
{
	if( KeyUp( key ) )
		return false;
	return ( FindKey( m_vecPrevKeys, key ) < 0 );
}

bool Input::KeyReleased( SDL_Scancode key )
{
	if( KeyDown( key ) )
		return false;
	return ( FindKey( m_vecPrevKeys, key ) >= 0 );
}

bool Input::KeyRepeated( SDL_Scancode key )
{
	return ( FindKey( m_vecRepeatKeys, key ) >= 0 );
}

bool Input::MBDown( Uint8 mb )
{
	return m_buttons[mb];
}

bool Input::MBUp( Uint8 mb )
{
	return !m_buttons[mb];
}

bool Input::MBPressed( Uint8 mb )
{
	if( MBUp( mb ) )
		return false;
	return !m_prevButtons[mb];
}

bool Input::MBReleased( Uint8 mb )
{
	if( MBDown( mb ) )
		return false;
	return m_prevButtons[mb];
}

vec2 Input::MousePosition() { return m_mousePosition; }
vec2 Input::MouseDelta() { return m_mouseDelta; }

// ***********************************************************************
// LUA
// ***********************************************************************

void Input::lua_Register( lua_State* lua )
{
	luaL_Reg funcs[] =
	{
		{ "KeyDown", lua_KeyDown },
		{ "KeyUp", lua_KeyUp },
		{ "KeyPressed", lua_KeyPressed },
		{ "KeyReleased", lua_KeyReleased },
		{ "KeyRepeated", lua_KeyRepeated },
		{ "MBDown", lua_MBDown },
		{ "MBUp", lua_MBUp },
		{ "MBPressed", lua_MBPressed },
		{ "MBReleased", lua_MBReleased },
		{ "MousePosition", lua_MousePosition },
		{ "MouseDelta", lua_MouseDelta },
		{ "StartTextInput", lua_StartTextInput },
		{ "StopTextInput", lua_StopTextInput },
		{ "TextInput", lua_TextInput },
		{ "TextInputEnabled", lua_TextInputEnabled },
		{ NULL, NULL }
	};

	lua_newtable( lua );
	luaL_setfuncs( lua, funcs, 0 );
	lua_setglobal( lua, "Input" );

	// register SDL scancodes (this is so ugly)
	lua_newtable( lua );
	int _KEYINDEX = 4;
	KEY( A ); KEY( B ); KEY( C ); KEY( D ); KEY( E ); KEY( F ); KEY( G ); KEY( H ); KEY( I );
	KEY( J ); KEY( K ); KEY( L ); KEY( M ); KEY( O ); KEY( P ); KEY( Q ); KEY( R ); KEY( S );
	KEY( S ); KEY( T ); KEY( U ); KEY( V ); KEY( W ); KEY( X );	KEY( Y ); KEY( Z );
	KEY( NUM_1 ); KEY( NUM_2 ); KEY( NUM_3 ); KEY( NUM_4 ); KEY( NUM_5 );
	KEY( NUM_6 ); KEY( NUM_7 ); KEY( NUM_8 ); KEY( NUM_9 ); KEY( NUM_0 );
	KEY( Return ); KEY( Escape ); KEY( Backspace ); KEY( Tab ); KEY( Space );
	KEY( Minus ); KEY( Equals ); KEY( LeftBracket ); KEY( RightBracket ); KEY( Backslash );
	KEY( Semicolon ); KEY( Apostrophe ); KEY( Grave ); KEY( Comma );
	KEY( Period ); KEY( Slash ); KEY( CapsLock );
	KEY( F1 ); KEY( F2 ); KEY( F3 ); KEY( F4 ); KEY( F5 ); KEY( F6 );
	KEY( F7 ); KEY( F8 ); KEY( F9 ); KEY( F10 ); KEY( F11 ); KEY( F12 );
	KEY( PrintScreen ); KEY( ScrollLock ); KEY( Pause ); KEY( Insert );
	KEY( Home ); KEY( PageUp ); KEY( Delete ); KEY( End ); KEY( PageDown );
	KEY( Right ); KEY( Left ); KEY( Down ); KEY( Up );
	KEY( KP_Divide ); KEY( KP_Multiply ); KEY( KP_Minus ); KEY( KP_Plus ); KEY( KP_Enter );
	KEY( KP_1 ); KEY( KP_2 ); KEY( KP_3 ); KEY( KP_4 ); KEY( KP_5 );
	KEY( KP_6  ); KEY( KP_7 ); KEY( KP_8 ); KEY( KP_9 ); KEY( KP_0 ); KEY( KP_Period );
	_KEYINDEX = 224;
	KEY( LeftControl ); KEY( LeftShift ); KEY( LeftAlt );
	KEY( RightControl ); KEY( RightShift ); KEY( RightAlt );
	lua_setglobal( lua, "Keys" );

	// register SDL mouse button (also ugly)
	lua_newtable( lua );
	BUTTON( Left, 0 );
	BUTTON( Middle, 1 );
	BUTTON( Right, 2 );
	lua_setglobal( lua, "Buttons" );
}

int Input::lua_KeyDown( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		SDL_Scancode key = static_cast<SDL_Scancode>( static_cast<int>( lua_tonumber( lua, 1 ) ) );
		lua_pushboolean( lua, Input::Instance().KeyDown( key ) );
		result = 1;
	}

	return result;
}

int Input::lua_KeyUp( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		SDL_Scancode key = static_cast<SDL_Scancode>( static_cast<int>( lua_tonumber( lua, 1 ) ) );
		lua_pushboolean( lua, Input::Instance().KeyUp( key ) );
		result = 1;
	}

	return result;
}

int Input::lua_KeyPressed( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		SDL_Scancode key = static_cast<SDL_Scancode>( static_cast<int>( lua_tonumber( lua, 1 ) ) );
		lua_pushboolean( lua, Input::Instance().KeyPressed( key ) );
		result = 1;
	}

	return result;
}

int Input::lua_KeyReleased( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		SDL_Scancode key = static_cast<SDL_Scancode>( static_cast<int>( lua_tonumber( lua, 1 ) ) );
		lua_pushboolean( lua, Input::Instance().KeyReleased( key ) );
		result = 1;
	}

	return result;
}

int Input::lua_KeyRepeated( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		SDL_Scancode key = static_cast<SDL_Scancode>( static_cast<int>( lua_tonumber( lua, 1 ) ) );
		lua_pushboolean( lua, Input::Instance().KeyRepeated( key ) );
		result = 1;
	}

	return result;
}

int Input::lua_MBDown( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		Uint8 mb = static_cast<Uint8>( lua_tonumber( lua, 1 ) );
		lua_pushboolean( lua, Input::Instance().MBDown( mb ) );
		result = 1;
	}

	return result;
}

int Input::lua_MBUp( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		Uint8 mb = static_cast<Uint8>( lua_tonumber( lua, 1 ) );
		lua_pushboolean( lua, Input::Instance().MBUp( mb ) );
		result = 1;
	}

	return result;
}

int Input::lua_MBPressed( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		Uint8 mb = static_cast<Uint8>( lua_tonumber( lua, 1 ) );
		lua_pushboolean( lua, Input::Instance().MBPressed( mb ) );
		result = 1;
	}

	return result;
}

int Input::lua_MBReleased( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		Uint8 mb = static_cast<Uint8>( lua_tonumber( lua, 1 ) );
		lua_pushboolean( lua, Input::Instance().MBReleased( mb ) );
		result = 1;
	}

	return result;
}

int Input::lua_MousePosition( lua_State* lua )
{
	return Vec2::lua_Write( lua, Input::Instance().MousePosition() );
}

int Input::lua_MouseDelta( lua_State* lua )
{
	return Vec2::lua_Write( lua, Input::Instance().MouseDelta() );
}

int Input::lua_StartTextInput( lua_State* lua )
{
	Input::Instance().StartTextInput();
	return 0;
}

int Input::lua_StopTextInput( lua_State* lua )
{
	Input::Instance().StopTextInput();
	return 0;
}

int Input::lua_TextInput( lua_State* lua )
{
	lua_pushstring( lua, Input::Instance().GetTextInput().c_str() );
	return 1;
}

int Input::lua_TextInputEnabled( lua_State* lua )
{
	lua_pushboolean( lua, Input::Instance().GetTextInputEnabled() );
	return 1;
}