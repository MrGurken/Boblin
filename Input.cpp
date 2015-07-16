#include "Input.h"

Input& Input::Instance()
{
	static Input instance;
	return instance;
}

void Input::SetWindow( SDL_Window* window ) { m_sdlWindow = window; }

bool Input::Update()
{
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
			//m_ssTextInput << e.text.text;
            int len = static_cast<int>( strlen(e.text.text) );
			for( int i=0; i<len; i++ )
				if( e.text.text[i] >= INPUT_ASCII_MIN && e.text.text[i] <= INPUT_ASCII_MAX )
					m_ssTextInput << e.text.text[i];
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

#ifdef WIN32
	for( int i=0; i<INPUT_MAX_GAMEPADS; i++ )
		if( m_gamepads[i].GetUsed() )
			m_gamepads[i].GetState();
#endif

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
    if( m_iTextInputs > 0 )
    {
        m_iTextInputs--;
        
        if( m_iTextInputs <= 0 )
        {
            SDL_StopTextInput();
            printf( "Input.cpp: Stopping text input.\n" );
        }
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

#ifdef WIN32
bool Input::GPConnected( int index ) { return m_gamepads[index].GetConnected(); }
bool Input::GPDown( int index, GamepadButton button ) { return m_gamepads[index].GPDown( button ); }
bool Input::GPUp( int index, GamepadButton button ) { return m_gamepads[index].GPUp( button ); }
bool Input::GPPressed( int index, GamepadButton button ) { return m_gamepads[index].GPPressed( button ); }
bool Input::GPReleased( int index, GamepadButton button ) { return m_gamepads[index].GPReleased( button ); }
float Input::LeftTrigger( int index ) { return m_gamepads[index].LeftTrigger(); }
float Input::RightTrigger( int index ) { return m_gamepads[index].RightTrigger(); }
vec2 Input::LeftStick( int index ) { return m_gamepads[index].LeftStick(); }
vec2 Input::RightStick( int index ) { return m_gamepads[index].RightStick(); }
vec2 Input::LeftStickDelta( int index ) { return m_gamepads[index].LeftStickDelta(); }
vec2 Input::RightStickDelta( int index ) { return m_gamepads[index].RightStickDelta(); }
void Input::Vibrate( int index, float left, float right ) { m_gamepads[index].Vibrate( left, right ); }
void Input::UseGamepad( int index, bool use ) { m_gamepads[index].SetUsed( use ); }
#endif

// ***********************************************************************
// LUA
// ***********************************************************************

void Input::lua_Register( lua_State* lua )
{
	// Keyboard
	luaL_Reg keyboardFuncs[] =
	{
		{ "Down", lua_KeyDown },
		{ "Up", lua_KeyUp },
		{ "Pressed", lua_KeyPressed },
		{ "Released", lua_KeyReleased },
		{ "Repeated", lua_KeyRepeated },
		{ "StartTextInput", lua_StartTextInput },
		{ "StopTextInput", lua_StopTextInput },
		{ "TextInput", lua_TextInput },
		{ "TextInputEnabled", lua_TextInputEnabled },
		{ NULL, NULL }
	};

	lua_newtable( lua );
	luaL_setfuncs( lua, keyboardFuncs, 0 );
	int _KEYINDEX = 4;
	KEY( A ); KEY( B ); KEY( C ); KEY( D ); KEY( E ); KEY( F ); KEY( G ); KEY( H ); KEY( I );
	KEY( J ); KEY( K ); KEY( L ); KEY( M ); KEY( O ); KEY( P ); KEY( Q ); KEY( R ); KEY( S );
	KEY( S ); KEY( T ); KEY( U ); KEY( V ); KEY( W ); KEY( X );	KEY( Y ); KEY( Z );
	KEY( NUM_1 ); KEY( NUM_2 ); KEY( NUM_3 ); KEY( NUM_4 ); KEY( NUM_5 );
	KEY( NUM_6 ); KEY( NUM_7 ); KEY( NUM_8 ); KEY( NUM_9 ); KEY( NUM_0 );
	KEY( Enter ); KEY( Escape ); KEY( Backspace ); KEY( Tab ); KEY( Space );
	KEY( Minus ); KEY( Equals ); KEY( LeftBracket ); KEY( RightBracket ); KEY( Backslash );
	KEY( Semicolon ); KEY( Apostrophe ); KEY( Grave ); KEY( Comma );
	KEY( Period ); KEY( Slash ); KEY( CapsLock );
	KEY( F1 ); KEY( F2 ); KEY( F3 ); KEY( F4 ); KEY( F5 ); KEY( F6 );
	KEY( F7 ); KEY( F8 ); KEY( F9 ); KEY( F10 ); KEY( F11 ); KEY( F12 );
	KEY( PrintScreen ); KEY( ScrollLock ); KEY( Pause ); KEY( Insert );
	KEY( Home ); KEY( PageUp ); KEY( Delete ); KEY( End ); KEY( PageDown );
	KEY( RightArrow ); KEY( LeftArrow ); KEY( DownArrow ); KEY( UpArrow );
	KEY( KP_Divide ); KEY( KP_Multiply ); KEY( KP_Minus ); KEY( KP_Plus ); KEY( KP_Enter );
	KEY( KP_1 ); KEY( KP_2 ); KEY( KP_3 ); KEY( KP_4 ); KEY( KP_5 );
	KEY( KP_6  ); KEY( KP_7 ); KEY( KP_8 ); KEY( KP_9 ); KEY( KP_0 ); KEY( KP_Period );
	_KEYINDEX = 224;
	KEY( LeftControl ); KEY( LeftShift ); KEY( LeftAlt );
	KEY( RightControl ); KEY( RightShift ); KEY( RightAlt );
	lua_setglobal( lua, "Keyboard" );

	// Mouse
	luaL_Reg mouseFuncs[] =
	{
		{ "Down", lua_MBDown },
		{ "Up", lua_MBUp },
		{ "Pressed", lua_MBPressed },
		{ "Released", lua_MBReleased },
		{ "Position", lua_MousePosition },
		{ "Delta", lua_MouseDelta },
		{ NULL, NULL }
	};

	lua_newtable( lua );
	luaL_setfuncs( lua, mouseFuncs, 0 );
	BUTTON( Left, 0 );
	BUTTON( Middle, 0 );
	BUTTON( Right, 0 );
	lua_setglobal( lua, "Mouse" );

#ifdef WIN32
	luaL_Reg gamepadFuncs[] =
	{
		{ "Use", lua_UseGamepad },
		{ "Connected", lua_GPConnected },
		{ "Down", lua_GPDown },
		{ "Up", lua_GPUp },
		{ "Pressed", lua_GPPressed },
		{ "Released", lua_GPReleased },
		{ "LeftTrigger", lua_LeftTrigger },
		{ "RightTrigger", lua_RightTrigger },
		{ "LeftStick", lua_LeftStick },
		{ "RightStick", lua_RightStick },
		{ "LeftStickDelta", lua_LeftStickDelta },
		{ "RightStickDelta", lua_RightStickDelta },
		{ "Vibrate", lua_Vibrate },
		{ NULL, NULL }
	};

	luaL_newmetatable( lua, "Gamepad" );
	luaL_setfuncs( lua, gamepadFuncs, 0 );
    lua_pushboolean( lua, true );
    lua_setfield( lua, -2, "available" );

	GP( DirUp, XINPUT_GAMEPAD_DPAD_UP );
	GP( DirDown, XINPUT_GAMEPAD_DPAD_DOWN );
	GP( DirLeft, XINPUT_GAMEPAD_DPAD_LEFT );
	GP( DirRight, XINPUT_GAMEPAD_DPAD_RIGHT );
	GP( Start, XINPUT_GAMEPAD_START );
	GP( Back, XINPUT_GAMEPAD_BACK );
	GP( LeftThumb, XINPUT_GAMEPAD_LEFT_THUMB );
	GP( RightThumb, XINPUT_GAMEPAD_RIGHT_THUMB );
	GP( LeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER );
	GP( RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER );
	GP( A, XINPUT_GAMEPAD_A );
	GP( B, XINPUT_GAMEPAD_B );
	GP( X, XINPUT_GAMEPAD_X );
	GP( Y, XINPUT_GAMEPAD_Y );

	lua_pushvalue( lua, -1 );
	lua_setfield( lua, -2, "__index" );
	lua_setglobal( lua, "Gamepad" );
#else
    lua_newtable( lua );
    lua_pushboolean( lua, false );
    lua_setfield( lua, -2, "available" );
    lua_setglobal( lua, "Gamepad" );
#endif
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

int Input::lua_ReadGamepad( lua_State* lua, int index )
{
	int result = -1;

	if( lua_istable( lua, index ) )
	{
		lua_getfield( lua, index, "index" );
		result = static_cast<int>( lua_tonumber( lua, -1 ) );
	}

	return result;
}

int Input::lua_WriteGamepad( lua_State* lua, int index )
{
	lua_newtable( lua );
	lua_pushnumber( lua, index );
	lua_setfield( lua, -2, "index" );
	luaL_getmetatable( lua, "Gamepad" );
	lua_setmetatable( lua, -2 );
	return 1;
}

#ifdef WIN32
int Input::lua_UseGamepad( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		int index = static_cast<int>( lua_tonumber( lua, 1 ) );
		bool use = true;
		if( nargs >= 2 )
			use = ( lua_toboolean( lua, 2 ) > 0 );

		Input::Instance().UseGamepad( index, use );
		if( use )
		{
			result = lua_WriteGamepad( lua, index );
		}
	}

	return result;
}

int Input::lua_GPConnected( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		lua_pushboolean( lua, Input::Instance().GPConnected( index ) );
		result = 1;
	}

	return result;
}

int Input::lua_GPDown( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 2 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		GamepadButton button = static_cast<GamepadButton>( static_cast<int>( lua_tonumber( lua, 2 ) ) );
		lua_pushboolean( lua, Input::Instance().GPDown( index, button ) );
		result = 1;
	}

	return result;
}

int Input::lua_GPUp( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 2 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		GamepadButton button = static_cast<GamepadButton>( static_cast<int>( lua_tonumber( lua, 2 ) ) );
		lua_pushboolean( lua, Input::Instance().GPUp( index, button ) );
		result = 1;
	}

	return result;
}

int Input::lua_GPPressed( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 2 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		GamepadButton button = static_cast<GamepadButton>( static_cast<int>( lua_tonumber( lua, 2 ) ) );
		lua_pushboolean( lua, Input::Instance().GPPressed( index, button ) );
		result = 1;
	}

	return result;
}

int Input::lua_GPReleased( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 2 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		GamepadButton button = static_cast<GamepadButton>( static_cast<int>( lua_tonumber( lua, 2 ) ) );
		lua_pushboolean( lua, Input::Instance().GPReleased( index, button ) );
		result = 1;
	}

	return result;
}

int Input::lua_LeftTrigger( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		lua_pushnumber( lua, Input::Instance().LeftTrigger( index ) );
		result = 1;
	}

	return result;
}

int Input::lua_RightTrigger( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		lua_pushnumber( lua, Input::Instance().RightTrigger( index ) );
		result = 1;
	}

	return result;
}

int Input::lua_LeftStick( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		result = Vec2::lua_Write( lua, Input::Instance().LeftStick( index ) );
	}

	return result;
}

int Input::lua_RightStick( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		result = Vec2::lua_Write( lua, Input::Instance().RightStick( index ) );
	}

	return result;
}

int Input::lua_LeftStickDelta( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		result = Vec2::lua_Write( lua, Input::Instance().LeftStickDelta( index ) );
	}

	return result;
}

int Input::lua_RightStickDelta( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		result = Vec2::lua_Write( lua, Input::Instance().RightStickDelta( index ) );
	}

	return result;
}

int Input::lua_Vibrate( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 3 )
	{
		int index = lua_ReadGamepad( lua, 1 );
		float left = static_cast<float>( lua_tonumber( lua, 2 ) );
		float right = static_cast<float>( lua_tonumber( lua, 3 ) );
		
		Input::Instance().Vibrate( index, left, right );
	}

	return result;
}
#endif

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