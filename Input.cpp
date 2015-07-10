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
	lua_register( lua, "KeyDown", lua_KeyDown );
	lua_register( lua, "KeyUp", lua_KeyUp );
	lua_register( lua, "KeyPressed", lua_KeyPressed );
	lua_register( lua, "KeyReleased", lua_KeyReleased );
	lua_register( lua, "MBDown", lua_MBDown );
	lua_register( lua, "MBUp", lua_MBUp );
	lua_register( lua, "MBPressed", lua_MBPressed );
	lua_register( lua, "MBReleased", lua_MBReleased );
	lua_register( lua, "MousePosition", lua_MousePosition );
	lua_register( lua, "MouseDelta", lua_MouseDelta );

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