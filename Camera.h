#ifndef CAMERA_H
#define CAMERA_H

#include "glm.hpp"
#include "gtc\matrix_transform.hpp"
#include "gtc\quaternion.hpp"
#include "gtx\transform.hpp"
using namespace glm;

class Camera
{
public:
	Camera();
	Camera( float aspectRatio, float fov, float near, float far );
	Camera( const Camera& ref );
	virtual ~Camera();

	void	Update();

	void	SetProjection( float aspectRatio, float fov, float near, float far );
	void	SetPosition( vec3 position );
	void	SetRotation( quat rotation );

	mat4	GetView() const;
	mat4	GetProjection() const;
	mat4	GetViewProjection() const;

private:
	vec3	m_position;
	quat	m_rotation;
	mat4	m_projection;
};

#endif