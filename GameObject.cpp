#include "GameObject.h"

vector<GameObject*> GameObject::s_vecObjects;
Mesh* GameObject::quad = nullptr;

GameObject::GameObject()
	: m_bAlive( false ), m_pTexture( nullptr )
{
	if( quad == nullptr )
	{
		Vertex vertices[] =
		{
			{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f, 1.0f, 1.0f }
		};

		GLuint indices[] =
		{
			0, 1, 2,
			1, 3, 2
		};

		quad = new Mesh();
		quad->AddVertices( vertices, 4, indices, 6 );
	}
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	m_position += m_velocity;
	m_velocity *= m_friction;
}

void GameObject::Render( Shader* shader )
{
	if( m_pTexture )
		m_pTexture->Bind();
	else
		glBindTexture( GL_TEXTURE_2D, 0 );

	mat4 world = translate( vec3( m_position.x, m_position.y, 0.0f ) ) * scale( vec3( m_renderBounds.w, m_renderBounds.h, 1.0f ) );
	shader->SetUniform( "ModelMatrix", world );

	// TODO: Figure out how to draw different kinds of meshes
	quad->Render();
}

bool GameObject::Collides( const GameObject& ref )
{
	return ( m_collisionBounds.Intersect( ref.m_collisionBounds ) );
}

void GameObject::UpdateAll()
{
	for( vector<GameObject*>::iterator it = s_vecObjects.begin(); it != s_vecObjects.end(); it++ )
		(*it)->Update();
}

void GameObject::RenderAll( Shader* shader )
{
	for( vector<GameObject*>::iterator it = s_vecObjects.begin(); it != s_vecObjects.end(); it++ )
		(*it)->Render( shader );
}

void GameObject::SetCollisionBounds( rect bounds ) { m_collisionBounds = bounds; }
void GameObject::SetRenderBounds( rect bounds ) { m_renderBounds = bounds; }
void GameObject::SetPosition( vec2 position ) { m_position = position; }
void GameObject::SetVelocity( vec2 velocity ) { m_velocity = velocity; }
void GameObject::SetFriction( vec2 friction ) { m_friction = friction; }
void GameObject::SetColor( vec4 color ) { m_color = color; }
void GameObject::SetTexture( Texture* texture ) { m_pTexture = texture; }
void GameObject::SetAlive( bool alive ) { m_bAlive = alive; }

rect GameObject::GetCollisionBounds() const { return m_collisionBounds; }
rect GameObject::GetRenderBounds() const { return m_renderBounds; }
vec2 GameObject::GetPosition() const { return m_position; }
vec2 GameObject::GetVelocity() const { return m_velocity; }
vec2 GameObject::GetFriction() const { return m_friction; }
vec4 GameObject::GetColor() const { return m_color; }
Texture* GameObject::GetTexture() const { return m_pTexture; }
bool GameObject::GetAlive() const { return m_bAlive; }

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
		{ "Texture", lua_Texture },
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
		result = static_cast<GameObject*>( lua_touserdata( lua, -1 ) );
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
	GameObject* ptr = new GameObject();
	s_vecObjects.push_back( ptr );
	return lua_Write( lua, ptr );
}

LIMP( Destroy )
{
	GameObject* ptr = lua_Read( lua, 1 );
	if( ptr )
	{
		vector<GameObject*>::iterator it;
		for( it = s_vecObjects.begin(); it != s_vecObjects.end(); it++ )
			if( *it == ptr )
				break;

		s_vecObjects.erase( it );
		delete ptr;
	}
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
			if( nargs >= 5 ) // setting floats
			{
				/*float x = static_cast<float>( lua_tonumber( lua, 2 ) );
				float y = static_cast<float>( lua_tonumber( lua, 3 ) );
				float w = static_cast<float>( lua_tonumber( lua, 4 ) );
				float h = static_cast<float>( lua_tonumber( lua, 5 ) );

				ptr->SetCollisionBounds( rect( x, y, w, h ) );*/
				ptr->SetCollisionBounds( rect::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetCollisionBounds( rect::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				rect r = ptr->GetCollisionBounds();
				/*lua_pushnumber( lua, r.x );
				lua_pushnumber( lua, r.y );
				lua_pushnumber( lua, r.w );
				lua_pushnumber( lua, r.h );
				result = 4;*/
				result = rect::lua_Write( lua, r );
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
				/*float x = static_cast<float>( lua_tonumber( lua, 2 ) );
				float y = static_cast<float>( lua_tonumber( lua, 3 ) );
				float w = static_cast<float>( lua_tonumber( lua, 4 ) );
				float h = static_cast<float>( lua_tonumber( lua, 5 ) );

				ptr->SetRenderBounds( rect( x, y, w, h ) );*/
				ptr->SetRenderBounds( rect::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting 
			{
				ptr->SetRenderBounds( rect::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				rect r = ptr->GetRenderBounds();
				/*lua_pushnumber( lua, r.x );
				lua_pushnumber( lua, r.y );
				lua_pushnumber( lua, r.w );
				lua_pushnumber( lua, r.h );
				result = 4;*/
				result = rect::lua_Write( lua, r );
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
				/*float x = static_cast<float>( lua_tonumber( lua, 2 ) );
				float y = static_cast<float>( lua_tonumber( lua, 3 ) );

				ptr->SetPosition( vec2( x, y ) );*/
				ptr->SetPosition( Vec2::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetPosition( Vec2::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				vec2 v = ptr->GetPosition();
				/*lua_pushnumber( lua, v.x );
				lua_pushnumber( lua, v.y );
				result = 2;*/
				result = Vec2::lua_Write( lua, v );
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
				/*float x = static_cast<float>( lua_tonumber( lua, 2 ) );
				float y = static_cast<float>( lua_tonumber( lua, 3 ) );

				ptr->SetVelocity( vec2( x, y ) );*/
				ptr->SetVelocity( Vec2::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetVelocity( Vec2::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				vec2 v = ptr->GetVelocity();
				/*lua_pushnumber( lua, v.x );
				lua_pushnumber( lua, v.y );
				result = 2;*/
				result = Vec2::lua_Write( lua, v );
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
				/*float x = static_cast<float>( lua_tonumber( lua, 2 ) );
				float y = static_cast<float>( lua_tonumber( lua, 3 ) );

				ptr->SetFriction( vec2( x, y ) );*/
				ptr->SetFriction( Vec2::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetFriction( Vec2::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				vec2 v = ptr->GetFriction();
				/*lua_pushnumber( lua, v.x );
				lua_pushnumber( lua, v.y );
				result = 2;*/
				result = Vec2::lua_Write( lua, v );
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
				/*float r = static_cast<float>( lua_tonumber( lua, 2 ) );
				float g = static_cast<float>( lua_tonumber( lua, 3 ) );
				float b = static_cast<float>( lua_tonumber( lua, 4 ) );
				float a = static_cast<float>( lua_tonumber( lua, 5 ) );

				ptr->SetColor( vec4( r, g, b, a ) );*/
				ptr->SetColor( Vec4::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetColor( Vec4::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				vec4 v = ptr->GetColor();
				/*lua_pushnumber( lua, v.r );
				lua_pushnumber( lua, v.g );
				lua_pushnumber( lua, v.b );
				lua_pushnumber( lua, v.a );
				result = 4;*/
				result = Vec4::lua_Write( lua, v );
			}
		}
	}

	return result;
}

LIMP( Texture )
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
				Texture* texture = Texture::lua_Read( lua, 2 );
				ptr->SetTexture( texture );
			}
			else // getting
			{
				Texture* texture = ptr->GetTexture();
				result = Texture::lua_Write( lua, texture );
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
				bool alive = ( lua_toboolean( lua, 2 ) != 0 );
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

LIMP( Collides )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 2 )
	{
		GameObject* a = lua_Read( lua, 1 );
		GameObject* b = lua_Read( lua, 2 );
		if( a && b )
		{
			lua_pushboolean( lua, a->Collides( *b ) );
			result = 1;
		}
	}

	return result;
}