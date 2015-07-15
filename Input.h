#ifndef INPUT_H
#define INPUT_H

#include "LibIncluder.h"
#include "maths.h"
#include "Gamepad.h"
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <sstream>
using std::stringstream;

#define INPUT_MAX_BUTTONS 3
#define KEY( _name ) lua_pushnumber( lua, _KEYINDEX++ ); lua_setfield( lua, -2, #_name );
#define BUTTON( _name, _index ) lua_pushnumber( lua, _index ); lua_setfield( lua, -2, #_name );
#define GP( _name, _index ) lua_pushnumber( lua, _index ); lua_setfield( lua, -2, #_name );
#define INPUT_ASCII_MIN 32
#define INPUT_ASCII_MAX 125
#define INPUT_MAX_GAMEPADS 4

class Input
{
public:
	static Input& Instance();
	virtual ~Input(){}

	void					SetWindow( SDL_Window* window );

	bool					Update();

	void					StartTextInput();
	void					StopTextInput();
	string					GetTextInput();
	bool					GetTextInputEnabled() const;

	bool					KeyDown( SDL_Scancode key );
	bool					KeyUp( SDL_Scancode key );
	bool					KeyPressed( SDL_Scancode key );
	bool					KeyReleased( SDL_Scancode key );
	bool					KeyRepeated( SDL_Scancode key );

	bool					MBDown( Uint8 mb );
	bool					MBUp( Uint8 mb );
	bool					MBPressed( Uint8 mb );
	bool					MBReleased( Uint8 mb );
	vec2					MousePosition();
	vec2					MouseDelta();

#ifdef WIN32
	void					UseGamepad( int index, bool use );
	bool					GPConnected( int index );
	bool					GPDown( int index, GamepadButton button );
	bool					GPUp( int index, GamepadButton button );
	bool					GPPressed( int index, GamepadButton button );
	bool					GPReleased( int index, GamepadButton button );
	float					LeftTrigger( int index );
	float					RightTrigger( int index );
	vec2					LeftStick( int index );
	vec2					RightStick( int index );
	vec2					LeftStickDelta( int index );
	vec2					RightStickDelta( int index );
	void					Vibrate( int index, float left, float right );
#endif

	static void				lua_Register( lua_State* lua );
	static int				lua_KeyDown( lua_State* lua );
	static int				lua_KeyUp( lua_State* lua );
	static int				lua_KeyPressed( lua_State* lua );
	static int				lua_KeyReleased( lua_State* lua );
	static int				lua_KeyRepeated( lua_State* lua );

	static int				lua_MBDown( lua_State* lua );
	static int				lua_MBUp( lua_State* lua );
	static int				lua_MBPressed( lua_State* lua );
	static int				lua_MBReleased( lua_State* lua );
	static int				lua_MousePosition( lua_State* lua );
	static int				lua_MouseDelta( lua_State* lua );

	static int				lua_ReadGamepad( lua_State* lua, int index );
	static int				lua_WriteGamepad( lua_State* lua, int index );
	static int				lua_UseGamepad( lua_State* lua );
	static int				lua_GPConnected( lua_State* lua );
	static int				lua_GPDown( lua_State* lua );
	static int				lua_GPUp( lua_State* lua );
	static int				lua_GPPressed( lua_State* lua );
	static int				lua_GPReleased( lua_State* lua );
	static int				lua_LeftTrigger( lua_State* lua );
	static int				lua_RightTrigger( lua_State* lua );
	static int				lua_LeftStick( lua_State* lua );
	static int				lua_RightStick( lua_State* lua );
	static int				lua_LeftStickDelta( lua_State* lua );
	static int				lua_RightStickDelta( lua_State* lua );
	static int				lua_Vibrate( lua_State* lua );

	static int				lua_StartTextInput( lua_State* lua );
	static int				lua_StopTextInput( lua_State* lua );
	static int				lua_TextInput( lua_State* lua );
	static int				lua_TextInputEnabled( lua_State* lua );

private:
	Input() : m_sdlWindow( nullptr ), m_iTextInputs( 0 )
	{
#ifdef WIN32
		for( int i=0; i<INPUT_MAX_GAMEPADS; i++ )
			m_gamepads[i].SetIndex( i );
#endif
	}
	Input( const Input& ref ){}

	int						FindKey( vector<SDL_Scancode>& vec, SDL_Scancode key );
	void					AddKey( vector<SDL_Scancode>& vec, SDL_Scancode key );
	void					RemoveKey( vector<SDL_Scancode>& vec, SDL_Scancode key );

	SDL_Window*				m_sdlWindow;
	vector<SDL_Scancode>	m_vecKeys;
	vector<SDL_Scancode>	m_vecPrevKeys;
	vector<SDL_Scancode>	m_vecRepeatKeys;
	bool					m_buttons[INPUT_MAX_BUTTONS];
	bool					m_prevButtons[INPUT_MAX_BUTTONS];
	vec2					m_mousePosition;
	vec2					m_mouseDelta;
	int						m_iMouseWheelDelta;
	int						m_iTextInputs;
	stringstream			m_ssTextInput;
#ifdef WIN32
	Gamepad					m_gamepads[INPUT_MAX_GAMEPADS];
#endif
};

#endif