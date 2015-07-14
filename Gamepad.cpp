#include "Gamepad.h"

Gamepad::Gamepad()
	: m_iIndex( -1 ), m_bConnected( false ), m_bUsed( false )
{
}

Gamepad::~Gamepad()
{
	if( m_iIndex >= 0 && m_iIndex < 4 )
	{
		XINPUT_VIBRATION vibration;
		ZeroMemory( &vibration, sizeof(vibration) );
		XInputSetState( m_iIndex, &vibration );
	}
}

void Gamepad::Vibrate( float left, float right )
{
	if( left < 0.0f ) left = 0.0f;
	else if( left > 1.0f ) left = 1.0f;
	if( right < 0.0f ) right = 0.0f;
	else if( right > 1.0f ) right = 1.0f;

	XINPUT_VIBRATION vibration;
	ZeroMemory( &vibration, sizeof(vibration) );

	vibration.wLeftMotorSpeed = static_cast<WORD>( left*65534 );
	vibration.wRightMotorSpeed = static_cast<WORD>( right*65534 );

	XInputSetState( m_iIndex, &vibration );
}

bool Gamepad::GPDown( GamepadButton button ) const
{
	return ( ( m_xState.Gamepad.wButtons & button ) > 0 );
}

bool Gamepad::GPUp( GamepadButton button ) const
{
	return  !GPDown( button );
}

bool Gamepad::GPPressed( GamepadButton button ) const
{
	if( GPUp( button ) )
		return false;
	return ( ( m_xPrevState.Gamepad.wButtons & button ) <= 0 );
}

bool Gamepad::GPReleased( GamepadButton button ) const
{
	if( GPDown( button ) )
		return false;
	return ( ( m_xPrevState.Gamepad.wButtons & button ) > 0 );
}

float Gamepad::LeftTrigger() const { return m_flLeftTrigger; }
float Gamepad::RightTrigger() const { return m_flRightTrigger; }
const vec2& Gamepad::LeftStick() const { return m_leftStick; }
const vec2& Gamepad::RightStick() const { return m_rightStick; }
const vec2& Gamepad::LeftStickDelta() const { return m_leftStickDelta; }
const vec2& Gamepad::RightStickDelta() const { return m_rightStickDelta; }
void Gamepad::SetIndex( int index ) { m_iIndex = index; }
void Gamepad::SetUsed( bool used ) { m_bUsed = used; }

const XINPUT_STATE& Gamepad::GetState()
{
	m_xPrevState = m_xState;

	ZeroMemory( &m_xState, sizeof(m_xState) );
	DWORD result = XInputGetState( m_iIndex, &m_xState );

	m_bConnected = ( result == ERROR_SUCCESS );
	if( m_bConnected )
	{
		m_flLeftTrigger = m_flRightTrigger = 0.0f;

		if( m_xState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
			m_flLeftTrigger = static_cast<float>( m_xState.Gamepad.bLeftTrigger ) / 255.0f;
		if( m_xState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
			m_flRightTrigger = static_cast<float>( m_xState.Gamepad.bRightTrigger ) / 255.0f;

		vec2 pleft = m_leftStick;
		vec2 pright = m_rightStick;

		m_leftStick.x = m_leftStick.y = m_rightStick.x = m_rightStick.y = 0.0f;

		if( m_xState.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			m_leftStick.x = static_cast<float>( m_xState.Gamepad.sThumbLX ) / 32768.0f;
		else if( m_xState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			m_leftStick.x = static_cast<float>( m_xState.Gamepad.sThumbLX ) / 32767.0f;
		if( m_xState.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			m_leftStick.y = static_cast<float>( m_xState.Gamepad.sThumbLY ) / 32768.0f;
		else if( m_xState.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			m_leftStick.y = static_cast<float>( m_xState.Gamepad.sThumbLY ) / 32767.0f;

		if( m_xState.Gamepad.sThumbRX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			m_rightStick.x = static_cast<float>( m_xState.Gamepad.sThumbRX ) / 32768.0f;
		else if( m_xState.Gamepad.sThumbRX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			m_rightStick.x = static_cast<float>( m_xState.Gamepad.sThumbRX ) / 32767.0f;
		if( m_xState.Gamepad.sThumbRY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			m_rightStick.y = static_cast<float>( m_xState.Gamepad.sThumbRY ) / 32768.0f;
		else if( m_xState.Gamepad.sThumbRY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			m_rightStick.y = static_cast<float>( m_xState.Gamepad.sThumbRY ) / 32767.0f;

		m_leftStickDelta =  m_leftStick - pleft;
		m_rightStickDelta = m_rightStick - pright;
	}

	return m_xState;
}

bool Gamepad::GetConnected() const { return m_bConnected; }
bool Gamepad::GetUsed() const { return m_bUsed; }