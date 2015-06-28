#include "Camera.h"

Camera* Camera::s_pActiveCamera = nullptr;

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

vec3 Camera::GetPosition() const
{
	return m_position;
}

quat Camera::GetRotation() const
{
	return m_rotation;
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

void Camera::SetActive( Camera* camera )
{
	s_pActiveCamera = camera;
}

Camera* Camera::GetActive()
{
	return s_pActiveCamera;
}

void Camera::lua_Register( lua_State* lua )
{
	luaL_Reg funcs[] =
	{
		{ "Create", lua_Create },
		{ "Projection", lua_Projection },
		{ "Position", lua_Position },
		{ "Rotation", lua_Rotation },
		{ "Active", lua_Active },
		{ NULL, NULL }
	};

	luaL_newmetatable( lua, "Camera" );
	luaL_setfuncs( lua, funcs, 0 );
	lua_pushvalue( lua, -1 );
	lua_setfield( lua, -1, "__index" );
	lua_setglobal( lua, "Camera" );
}

Camera* Camera::lua_Read( lua_State* lua, int index )
{
	Camera* result = nullptr;

	if( lua_istable( lua, index ) )
	{
		lua_getfield( lua, index, "__self" );
		result = reinterpret_cast<Camera*>( lua_touserdata( lua, -1 ) );
	}

	return result;
}

int Camera::lua_Write( lua_State* lua, Camera* camera )
{
	lua_newtable( lua );
	lua_pushlightuserdata( lua, camera );
	lua_setfield( lua, -2, "__self" );
	luaL_getmetatable( lua, "Camera" );
	lua_setmetatable( lua, -2 );
	return 1;
}

int Camera::lua_Create( lua_State* lua )
{
	Camera* camera = new Camera();
	return lua_Write( lua, camera );
}

int Camera::lua_Projection( lua_State* lua )
{
	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Camera* camera = lua_Read( lua, 1 );
		if( camera )
		{
			if( nargs >= 6 ) // orthographics
			{
				camera->SetProjection( lua_tonumber( lua, 2 ), lua_tonumber( lua, 3 ),
									   lua_tonumber( lua, 4 ), lua_tonumber( lua, 5 ),
									   lua_tonumber( lua, 6 ), lua_tonumber( lua, 7 ) );
			}
			else if( nargs >= 4 ) // perspective
			{
				camera->SetProjection( lua_tonumber( lua, 2 ), lua_tonumber( lua, 3 ),
									   lua_tonumber( lua, 4 ), lua_tonumber( lua, 5 ) );
			}
		}
	}

	return 0;
}

int Camera::lua_Position( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Camera* camera = lua_Read( lua, 1 );
		if( camera )
		{
			if( nargs >= 4 ) // setting
			{
				camera->SetPosition( vec3( lua_tonumber( lua, 2 ), lua_tonumber( lua, 3 ), lua_tonumber( lua, 4 ) ) );
			}
			else // getting
			{
				vec3 pos = camera->GetPosition();
				lua_pushnumber( lua, pos.x );
				lua_pushnumber( lua, pos.y );
				lua_pushnumber( lua, pos.z );
				result = 3;
			}
		}
	}

	return result;
}

int Camera::lua_Rotation( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Camera* camera = lua_Read( lua, 1 );
		if( camera )
		{
			if( nargs >= 5 ) // setting
			{
				camera->SetRotation( quat( lua_tonumber( lua, 2 ), lua_tonumber( lua, 3 ),
											lua_tonumber( lua, 4 ), lua_tonumber( lua, 5 ) ) );
			}
			else // getting
			{
				quat rot = camera->GetRotation();
				lua_pushnumber( lua, rot.x );
				lua_pushnumber( lua, rot.y );
				lua_pushnumber( lua, rot.z );
				lua_pushnumber( lua, rot.w );
				result = 4;
			}
		}
	}

	return result;
}

int Camera::lua_Active( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 ) // setting
	{
		Camera* camera = lua_Read( lua, 1 );
		if( camera )
			SetActive( camera );
	}
	else // getting
	{
		if( s_pActiveCamera )
		{
			result = lua_Write( lua, s_pActiveCamera );
		}
	}

	return result;
}