#ifndef GAMEPAD_H
#define GAMEPAD_H

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Xinput.h>
#include "glm.hpp"
using namespace glm;

enum GamepadButton
{
	Up				= XINPUT_GAMEPAD_DPAD_UP,
	Down			= XINPUT_GAMEPAD_DPAD_DOWN,
	Left			= XINPUT_GAMEPAD_DPAD_LEFT,
	Right			= XINPUT_GAMEPAD_DPAD_RIGHT,
	Start			= XINPUT_GAMEPAD_START,
	Back			= XINPUT_GAMEPAD_BACK,
	LeftThumb		= XINPUT_GAMEPAD_LEFT_THUMB,
	RightThumb		= XINPUT_GAMEPAD_RIGHT_THUMB,
	LeftShoulder	= XINPUT_GAMEPAD_LEFT_SHOULDER,
	RightShoulder	= XINPUT_GAMEPAD_RIGHT_SHOULDER,
	A				= XINPUT_GAMEPAD_A,
	B				= XINPUT_GAMEPAD_B,
	X				= XINPUT_GAMEPAD_X,
	Y				= XINPUT_GAMEPAD_Y,
};

class Gamepad
{
public:
	Gamepad();
	virtual ~Gamepad();

	void				Vibrate( float left, float right );

	bool				GPDown( GamepadButton button ) const;
	bool				GPUp( GamepadButton button ) const;
	bool				GPPressed( GamepadButton button ) const;
	bool				GPReleased( GamepadButton button ) const;

	float				LeftTrigger() const;
	float				RightTrigger() const;

	const vec2&			LeftStick() const;
	const vec2&			RightStick() const;
	const vec2&			LeftStickDelta() const;
	const vec2&			RightStickDelta() const;

	void				SetIndex( int index );
	void				SetUsed( bool used );

	const XINPUT_STATE&	GetState();
	bool				GetConnected() const;
	bool				GetUsed() const;

private:
	XINPUT_STATE		m_xState;
	XINPUT_STATE		m_xPrevState;
	int					m_iIndex;
	bool				m_bConnected;
	bool				m_bUsed;
	float				m_flLeftTrigger;
	float				m_flRightTrigger;
	vec2				m_leftStick;
	vec2				m_rightStick;
	vec2				m_leftStickDelta;
	vec2				m_rightStickDelta;
};

#else
#endif

#endif