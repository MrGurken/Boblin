#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera( float aspectRatio, float fov, float near, float far )
{
	SetProjection( aspectRatio, fov, near, far );
}

Camera::Camera( const Camera& ref )
	: m_position( ref.m_position ), m_projection( ref.m_projection )
{
}

void Camera::Update()
{
}

void Camera::SetProjection( float aspectRatio, float fov, float near, float far )
{
	m_projection = glm::perspective( fov, aspectRatio, near, far );
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
	mat4 result = mat4_cast( m_rotation ) * translate( -m_position );
	return result;
}

mat4 Camera::GetProjection() const
{
	return m_projection;
}

mat4 Camera::GetViewProjection() const
{
	return ( GetView() * m_projection );
}