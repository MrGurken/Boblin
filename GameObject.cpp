#include "GameObject.h"

GameObject::GameObject()
	: m_bAlive( false )
{
}

GameObject::~GameObject()
{
}

void GameObject::SetCollisionBounds( rect bounds ) { m_collisionBounds = bounds; }
void GameObject::SetRenderBounds( rect bounds ) { m_renderBounds = bounds; }
void GameObject::SetPosition( vec2 position ) { m_position = position; }
void GameObject::SetVelocity( vec2 velocity ) { m_velocity = velocity; }
void GameObject::SetFriction( vec2 friction ) { m_friction = friction; }
void GameObject::SetColor( vec4 color ) { m_color = color; }
void GameObject::SetAlive( bool alive ) { m_bAlive = alive; }

rect GameObject::GetCollisionBounds() const { return m_collisionBounds; }
rect GameObject::GetRenderBounds() const { return m_renderBounds; }
vec2 GameObject::GetPosition() const { return m_position; }
vec2 GameObject::GetVelocity() const { return m_velocity; }
vec2 GameObject::GetFriction() const { return m_friction; }
bool GameObject::GetAlive() const { return m_bAlive; }
vec4 GameObject::GetColor() const { return m_color; }

void GameObject::lua_Register( lua_State* lua )
{
	luaL_Reg funcs[] =
	{
		{ "Create", lua_Create },
		{ "Destroy", lua_Destroy },
		{ "CollisionBounds", lua_CollisionBounds },
		{ "RenderBounds", lua_RenderBounds },
		{ "Position", lua_Position },
		{ "Velocity", lua_Velocity },
		{ "Friction", lua_Friction },
		{ "Color", lua_Color },
		{ "Alive", lua_Alive },
		{ NULL, NULL }
	};

	luaL_newmetatable( lua, "GameObject" );
	luaL_setfuncs( lua, funcs, 0 );
	lua_pushvalue( lua, -1 );
	lua_setfield( lua, -2, "__index" );
	lua_setglobal( lua, "GameObject" );
}

GameObject* GameObject::lua_Read( lua_State* lua, int index )
{
	GameObject* result = nullptr;
	if( lua_istable( lua, index ) )
	{
		lua_getfield( lua, index, "__self" );
		result = (GameObject*)lua_touserdata( lua, -1 );
		lua_pop( lua, 1 );
	}

	return result;
}

int GameObject::lua_Write( lua_State* lua, GameObject* object )
{
	lua_newtable( lua );
	lua_pushlightuserdata( lua, object );
	lua_setfield( lua, -2, "__self" );
	luaL_setmetatable( lua, "GameObject" );
	return 1;
}

LIMP( Create )
{
	// TODO: Add to global vector of objects
	GameObject* ptr = new GameObject();
	return lua_Write( lua, ptr );
}

LIMP( Destroy )
{
	// TODO: Remove from global vector of objects
	GameObject* ptr = lua_Read( lua, 1 );
	if( ptr )
		delete ptr;
	return 0;
}

LIMP( CollisionBounds )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 ) // need atleast the game object itself to either set or get
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 5 ) // setting
			{
				float x = lua_tonumber( lua, 2 );
				float y = lua_tonumber( lua, 3 );
				float w = lua_tonumber( lua, 4 );
				float h = lua_tonumber( lua, 5 );

				ptr->SetCollisionBounds( rect( x, y, w, h ) );
			}
			else // getting
			{
				rect r = ptr->GetCollisionBounds();
				lua_pushnumber( lua, r.x );
				lua_pushnumber( lua, r.y );
				lua_pushnumber( lua, r.w );
				lua_pushnumber( lua, r.h );
				result = 4;
			}
		}
	}

	return result;
}

LIMP( RenderBounds )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 ) 
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 5 ) // setting
			{
				float x = lua_tonumber( lua, 2 );
				float y = lua_tonumber( lua, 3 );
				float w = lua_tonumber( lua, 4 );
				float h = lua_tonumber( lua, 5 );

				ptr->SetRenderBounds( rect( x, y, w, h ) );
			}
			else
			{
				rect r = ptr->GetRenderBounds();
				lua_pushnumber( lua, r.x );
				lua_pushnumber( lua, r.y );
				lua_pushnumber( lua, r.w );
				lua_pushnumber( lua, r.h );
				result = 4;
			}
		}
	}

	return result;
}

LIMP( Position )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 3 ) // setting
			{
				float x = lua_tonumber( lua, 2 );
				float y = lua_tonumber( lua, 3 );

				ptr->SetPosition( vec2( x, y ) );
			}
			else // getting
			{
				vec2 v = ptr->GetPosition();
				lua_pushnumber( lua, v.x );
				lua_pushnumber( lua, v.y );
				result = 2;
			}
		}
	}

	return result;
}

LIMP( Velocity )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 3 ) // setting
			{
				float x = lua_tonumber( lua, 2 );
				float y = lua_tonumber( lua, 3 );

				ptr->SetVelocity( vec2( x, y ) );
			}
			else // getting
			{
				vec2 v = ptr->GetVelocity();
				lua_pushnumber( lua, v.x );
				lua_pushnumber( lua, v.y );
				result = 2;
			}
		}
	}

	return result;
}

LIMP( Friction )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 3 ) // setting
			{
				float x = lua_tonumber( lua, 2 );
				float y = lua_tonumber( lua, 3 );

				ptr->SetFriction( vec2( x, y ) );
			}
			else // getting
			{
				vec2 v = ptr->GetFriction();
				lua_pushnumber( lua, v.x );
				lua_pushnumber( lua, v.y );
				result = 2;
			}
		}
	}

	return result;
}

LIMP( Alive )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 2 ) // setting
			{
				bool alive = lua_toboolean( lua, 2 );
				ptr->SetAlive( alive );
			}
			else // getting
			{
				lua_pushboolean( lua, ptr->GetAlive() );
				result = 1;
			}
		}
	}

	return result;
}

LIMP( Color )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 5 ) // setting
			{
				float r = lua_tonumber( lua, 2 );
				float g = lua_tonumber( lua, 3 );
				float b = lua_tonumber( lua, 4 );
				float a = lua_tonumber( lua, 5 );

				ptr->SetColor( vec4( r, g, b, a ) );
			}
			else // getting
			{
				vec4 v = ptr->GetColor();
				lua_pushnumber( lua, v.r );
				lua_pushnumber( lua, v.g );
				lua_pushnumber( lua, v.b );
				lua_pushnumber( lua, v.a );
				result = 4;
			}
		}
	}

	return result;
}