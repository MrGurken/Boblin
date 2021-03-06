#ifndef CAMERA_H
#define CAMERA_H

#include "Config.h"

#include "maths.h"
#include "lua.hpp"
#include "glm.hpp"
#include "matrix_transform.hpp"
#include "quaternion.hpp"
#include "transform.hpp"
using namespace glm;

class Camera
{
public:
	Camera();
	Camera( float fov, float aspectRatio, float nearplane, float farplane );
	Camera( float left, float right, float top, float bottom, float nearplane, float farplane );
	Camera( const Camera& ref );
	virtual ~Camera();

	void			Update();
	void			Center( vec2 position );

	void			SetProjection( float fov, float aspectRatio, float nearplane, float farplane );
	void			SetProjection( float left, float right, float top, float bottom, float nearplane, float farplane );
	void			SetProjection( const mat4& projection );
	void			SetPosition( vec3 position );
	void			SetRotation( quat rotation );

	vec3			GetPosition() const;
	quat			GetRotation() const;
	mat4			GetView() const;
	mat4			GetProjection() const;
	mat4			GetViewProjection() const;

	static void		SetActive( Camera* camera );
	static Camera*	GetActive();

	// lua
	static void		lua_Register( lua_State* lua );
	static Camera*	lua_Read( lua_State* lua, int index );
	static int		lua_Write( lua_State* lua, Camera* camera );
	static int		lua_Create( lua_State* lua );
	static int		lua_Projection( lua_State* lua );
	static int		lua_Position( lua_State* lua );
	static int		lua_Center( lua_State* lua );
	static int		lua_Rotation( lua_State* lua );
	static int		lua_Active( lua_State* lua );

private:
	vec3			m_position;
	quat			m_rotation;
	mat4			m_projection;

	static Camera*	s_pActiveCamera;
};

#endif