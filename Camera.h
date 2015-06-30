#ifndef CAMERA_H
#define CAMERA_H

#include "lua.hpp"
#include "glm.hpp"
#ifdef WIN32
#include "gtc\matrix_transform.hpp"
#include "gtc\quaternion.hpp"
#include "gtx\transform.hpp"
#else
#include "matrix_transform.hpp"
#include "quaternion.hpp"
#include "transform.hpp"
#endif
using namespace glm;

class Camera
{
public:
	Camera();
	Camera( float aspectRatio, float fov, float nearplane, float farplane );
	Camera( float left, float right, float top, float bottom, float nearplane, float farplane );
	Camera( const Camera& ref );
	virtual ~Camera();

	void			Update();

	void			SetProjection( float aspectRatio, float fov, float nearplane, float farplane );
	void			SetProjection( float left, float right, float top, float bottom, float nearplane, float farplane );
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
	static int		lua_Rotation( lua_State* lua );
	static int		lua_Active( lua_State* lua );

private:
	vec3			m_position;
	quat			m_rotation;
	mat4			m_projection;

	static Camera*	s_pActiveCamera;
};

#endif