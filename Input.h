#ifndef INPUT_H
#define INPUT_H

#include "LibIncluder.h"
#include "maths.h"
#include <vector>
using std::vector;

#define INPUT_MAX_BUTTONS 3

class Input
{
public:
	static Input& Instance();
	virtual ~Input(){}

	void				SetWindow( SDL_Window* window );

	bool				Update();

	bool				KeyDown( SDL_Keycode key );
	bool				KeyUp( SDL_Keycode key );
	bool				KeyPressed( SDL_Keycode key );
	bool				KeyReleased( SDL_Keycode key );
	bool				MBDown( Uint8 mb );
	bool				MBUp( Uint8 mb );
	bool				MBPressed( Uint8 mb );
	bool				MBReleased( Uint8 mb );

	vec2				MousePosition();
	vec2				MouseDelta();

	static void			lua_Register( lua_State* lua );
	static int			lua_KeyDown( lua_State* lua );
	static int			lua_KeyUp( lua_State* lua );
	static int			lua_KeyPressed( lua_State* lua );
	static int			lua_KeyReleased( lua_State* lua );
	static int			lua_MBDown( lua_State* lua );
	static int			lua_MBUp( lua_State* lua );
	static int			lua_MBPressed( lua_State* lua );
	static int			lua_MBReleased( lua_State* lua );
	static int			lua_MousePosition( lua_State* lua );
	static int			lua_MouseDelta( lua_State* lua );

private:
	Input() : m_sdlWindow( nullptr ) {}
	Input( const Input& ref ){}

	int					FindKey( vector<SDL_Keycode>& vec, SDL_Keycode key );
	void				AddKey( vector<SDL_Keycode>& vec, SDL_Keycode key );
	void				RemoveKey( vector<SDL_Keycode>& vec, SDL_Keycode key );

	SDL_Window*			m_sdlWindow;
	vector<SDL_Keycode>	m_vecKeys;
	vector<SDL_Keycode> m_vecPrevKeys;
	vector<SDL_Keycode> m_vecRepeatKeys;
	bool				m_buttons[INPUT_MAX_BUTTONS];
	bool				m_prevButtons[INPUT_MAX_BUTTONS];
	vec2				m_mousePosition;
	vec2				m_mouseDelta;
	int					m_iMouseWheelDelta;
};

#endif