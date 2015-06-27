#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera( float aspectRatio, float fov, float nearplane, float farplane )
{
	SetProjection( aspectRatio, fov, nearplane, farplane );
}

Camera::Camera( float left, float right, float top, float bottom, float nearplane, float farplane )
{
	SetProjection( left, right, top, bottom, nearplane, farplane );
}

Camera::Camera( const Camera& ref )
	: m_position( ref.m_position ), m_projection( ref.m_projection )
{
}

Camera::~Camera()
{
}

void Camera::Update()
{
}

void Camera::SetProjection( float aspectRatio, float fov, float nearplane, float farplane )
{
	m_projection = perspective( fov, aspectRatio, nearplane, farplane );
}

void Camera::SetProjection( float left, float right, float top, float bottom, float nearplane, float farplane )
{
	m_projection = ortho( left, right, bottom, top, nearplane, farplane );
}

void Camera::SetPosition( vec3 position )
{
	m_position = position;
}

void Camera::SetRotation( quat rotation )
{
	m_rotation = rotation;
}

mat4 Camera::GetView() const
{
	mat4 result = /*mat4_cast( m_rotation ) **/ translate( -m_position );
	return result;
}

mat4 Camera::GetProjection() const
{
	return m_projection;
}

mat4 Camera::GetViewProjection() const
{
	return ( m_projection * GetView() );
}