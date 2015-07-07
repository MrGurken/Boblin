#include "Camera.h"

Camera* Camera::s_pActiveCamera = nullptr;

Camera::Camera()
{
}

Camera::Camera( float fov, float aspectRatio, float nearplane, float farplane )
{
	SetProjection( fov, aspectRatio, nearplane, farplane );
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

void Camera::Center( vec2 position )
{
	m_position.x = position.x - Config::Instance().GetWidth() * 0.5f;
	m_position.y = position.y - Config::Instance().GetHeight() * 0.5f;
}

void Camera::SetProjection( float fov, float aspectRatio, float nearplane, float farplane )
{
	m_projection = perspective( fov, aspectRatio, nearplane, farplane );
}

void Camera::SetProjection( float left, float right, float top, float bottom, float nearplane, float farplane )
{
	m_projection = ortho( left, right, bottom, top, nearplane, farplane );
}

void Camera::SetProjection( const mat4& projection )
{
	m_projection = projection;
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
		{ "Center", lua_Center },
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
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Camera* camera = lua_Read( lua, 1 );
		if( camera )
		{
			if( nargs >= 6 ) // orthographic
			{
				camera->SetProjection( static_cast<float>( lua_tonumber( lua, 2 ) ),
									   static_cast<float>( lua_tonumber( lua, 3 ) ),
									   static_cast<float>( lua_tonumber( lua, 4 ) ),
									   static_cast<float>( lua_tonumber( lua, 5 ) ),
									   static_cast<float>( lua_tonumber( lua, 6 ) ),
									   static_cast<float>( lua_tonumber( lua, 7 ) ) );
			}
			else if( nargs >= 4 ) // perspective
			{
				camera->SetProjection( static_cast<float>( lua_tonumber( lua, 2 ) ),
									   static_cast<float>( lua_tonumber( lua, 3 ) ),
									   static_cast<float>( lua_tonumber( lua, 4 ) ),
									   static_cast<float>( lua_tonumber( lua, 5 ) ) );
			}
			else if( nargs >= 2 ) // set matrix
			{
				mat4 projectionMatrix = Mat4::lua_Read( lua, 2 );
				camera->m_projection = projectionMatrix;
			}
			else // get matrix
			{
				result = Mat4::lua_Write( lua, camera->GetProjection() );
			}
		}
	}

	return result;
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
			if( nargs >= 4 ) // setting floats
			{
				camera->SetPosition( vec3( static_cast<float>( lua_tonumber( lua, 2 ) ),
											static_cast<float>( lua_tonumber( lua, 3 ) ),
											static_cast<float>( lua_tonumber( lua, 4 ) ) ) );
			}
			else if( nargs >= 2 ) // setting vec3
			{
				camera->SetPosition( Vec3::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				result = Vec3::lua_Write( lua, camera->GetPosition() );
			}
		}
	}

	return result;
}

int Camera::lua_Center( lua_State* lua )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		Camera* camera = lua_Read( lua, 1 );
		if( camera )
		{
			if( nargs >= 3 ) // setting floats
			{
				camera->Center( Vec2::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting vec2
			{
				camera->Center( Vec2::lua_Read( lua, 2 ) );
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
			if( nargs >= 5 ) // setting floats
			{
				camera->SetRotation( quat( static_cast<float>( lua_tonumber( lua, 2 ) ),
											static_cast<float>( lua_tonumber( lua, 3 ) ),
											static_cast<float>( lua_tonumber( lua, 4 ) ),
											static_cast<float>( lua_tonumber( lua, 5 ) ) ) );
			}
			else if( nargs >= 2 ) // setting quat
			{
				camera->SetRotation( Quat::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				quat rot = camera->GetRotation();
				result = Quat::lua_Write( lua, rot );
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