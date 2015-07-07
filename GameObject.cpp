#include "GameObject.h"

vector<GameObject*> GameObject::s_vecObjects;
Mesh* GameObject::quad = nullptr;

GameObject::GameObject()
	: m_bAlive( false ), m_pTexture( nullptr ),
	m_collisionBounds( 0, 0, 32, 32 ), m_renderBounds( 0, 0, 32, 32 ),
	m_uvMin( 0, 0 ), m_uvLength( 1, 1 ), m_friction( 1, 1 )
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

GameObject::GameObject( const GameObject& ref )
	: m_collisionBounds( ref.m_collisionBounds ), m_renderBounds( ref.m_renderBounds ),
	m_position( ref.m_position ), m_velocity( ref.m_velocity ), m_friction( ref.m_friction ),
	m_color( ref.m_color ), m_pTexture( ref.m_pTexture ),
	m_uvMin( ref.m_uvMin ), m_uvLength( ref.m_uvLength ), m_bAlive( ref.m_bAlive )
{
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

	mat4 world = translate( vec3( m_position.x+m_renderBounds.x, m_position.y+m_renderBounds.y, 0.0f ) ) * scale( vec3( m_renderBounds.w, m_renderBounds.h, 1.0f ) );
	shader->SetUniform( "ModelMatrix", world );
	shader->SetUniform( "UVMin", m_uvMin );
	shader->SetUniform( "UVLength", m_uvLength );

	quad->Render();
}

bool GameObject::Collides( const GameObject& ref )
{
	return ( GetWorldBounds().Intersect( ref.GetWorldBounds() ) );
}

void GameObject::UpdateAll()
{
	for( vector<GameObject*>::iterator it = s_vecObjects.begin(); it != s_vecObjects.end(); it++ )
        if( (*it)->GetAlive() )
            (*it)->Update();
}

void GameObject::RenderAll( Shader* shader )
{
	for( vector<GameObject*>::iterator it = s_vecObjects.begin(); it != s_vecObjects.end(); it++ )
        if( (*it)->GetAlive() )
            (*it)->Render( shader );
}

void GameObject::SetCollisionBounds( rect bounds ) { m_collisionBounds = bounds; }
void GameObject::SetRenderBounds( rect bounds ) { m_renderBounds = bounds; }
void GameObject::SetPosition( vec2 position ) { m_position = position; }
void GameObject::SetVelocity( vec2 velocity ) { m_velocity = velocity; }
void GameObject::SetFriction( vec2 friction ) { m_friction = friction; }
void GameObject::SetColor( vec4 color ) { m_color = color; }
void GameObject::SetTexture( Texture* texture ) { m_pTexture = texture; }
void GameObject::SetCrop( rect crop )
{
	if( m_pTexture )
	{
		m_uvMin.x = crop.x / m_pTexture->GetWidth();
		m_uvMin.y = crop.y / m_pTexture->GetHeight();
		m_uvLength.x = crop.w / m_pTexture->GetWidth();
		m_uvLength.y = crop.h / m_pTexture->GetHeight();
	}
	else
		printf( "GameObject.cpp: Can't set crop without texture.\n" );
}
void GameObject::SetUVMin( vec2 uv ) { m_uvMin = uv; }
void GameObject::SetUVLength( vec2 uv ) { m_uvLength = uv; }
void GameObject::SetAlive( bool alive ) { m_bAlive = alive; }

rect GameObject::GetCollisionBounds() const { return m_collisionBounds; }
rect GameObject::GetRenderBounds() const { return m_renderBounds; }
rect GameObject::GetWorldBounds() const { return rect( m_collisionBounds.x+m_position.x, m_collisionBounds.y+m_position.y, m_collisionBounds.w, m_collisionBounds.h ); }
vec2 GameObject::GetPosition() const { return m_position; }
vec2 GameObject::GetVelocity() const { return m_velocity; }
vec2 GameObject::GetFriction() const { return m_friction; }
vec4 GameObject::GetColor() const { return m_color; }
Texture* GameObject::GetTexture() const { return m_pTexture; }
rect GameObject::GetCrop() const
{
	rect result;
	if( m_pTexture )
	{
		result.x = m_uvMin.x * m_pTexture->GetWidth();
		result.y = m_uvMin.y * m_pTexture->GetHeight();
		result.w = m_uvLength.x * m_pTexture->GetWidth();
		result.h = m_uvLength.y * m_pTexture->GetHeight();
	}
	else
		printf( "GameObject.cpp: Can't get crop without texture.\n" );

	return result;
}
vec2 GameObject::GetUVMin() const { return m_uvMin; }
vec2 GameObject::GetUVLength() const { return m_uvLength; }
bool GameObject::GetAlive() const { return m_bAlive; }

void GameObject::lua_Register( lua_State* lua )
{
	luaL_Reg funcs[] =
	{
		{ "Create", lua_Create },
		{ "Copy", lua_Copy },
		{ "Destroy", lua_Destroy },
		{ "CollisionBounds", lua_CollisionBounds },
		{ "RenderBounds", lua_RenderBounds },
		{ "WorldBounds", lua_WorldBounds },
		{ "Position", lua_Position },
		{ "Velocity", lua_Velocity },
		{ "Friction", lua_Friction },
		{ "Color", lua_Color },
		{ "Texture", lua_Texture },
		{ "Crop", lua_Crop },
		{ "UVMin", lua_UVMin },
		{ "UVLength", lua_UVLength },
		{ "Alive", lua_Alive },
		{ "Collides", lua_Collides },
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

LIMP( Copy )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			GameObject* newptr = new GameObject( *ptr );
			s_vecObjects.push_back( newptr );
			result = lua_Write( lua, newptr );
		}
	}

	return result;
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
	if( nargs >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 5 ) // setting floats
			{
				ptr->SetCollisionBounds( rect::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetCollisionBounds( rect::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				rect r = ptr->GetCollisionBounds();
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
				ptr->SetRenderBounds( rect::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting 
			{
				ptr->SetRenderBounds( rect::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				rect r = ptr->GetRenderBounds();
				result = rect::lua_Write( lua, r );
			}
		}
	}

	return result;
}

LIMP( WorldBounds )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			result = rect::lua_Write( lua, ptr->GetWorldBounds() );
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
				ptr->SetPosition( Vec2::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetPosition( Vec2::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				vec2 v = ptr->GetPosition();
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
				ptr->SetVelocity( Vec2::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetVelocity( Vec2::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				vec2 v = ptr->GetVelocity();
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
				ptr->SetFriction( Vec2::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetFriction( Vec2::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				vec2 v = ptr->GetFriction();
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
				ptr->SetColor( Vec4::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetColor( Vec4::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				vec4 v = ptr->GetColor();
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

LIMP( Crop )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 5 ) // setting componentwise
			{
				ptr->SetCrop( rect::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetCrop( rect::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				result = rect::lua_Write( lua, ptr->GetCrop() );
			}
		}
	}

	return result;
}

LIMP( UVMin )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 3 ) // setting componentwise
			{
				ptr->SetUVMin( Vec2::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetUVLength( Vec2::lua_Read( lua, 2 ) );
			}
			else // getting
			{
				result = Vec2::lua_Write( lua, ptr->GetUVMin() );
			}
		}
	}

	return result;
}

LIMP( UVLength )
{
	int result = 0;

	int nargs = lua_gettop( lua );
	if( nargs >= 1 )
	{
		GameObject* ptr = lua_Read( lua, 1 );
		if( ptr )
		{
			if( nargs >= 3 ) // setting componentwise
			{
				ptr->SetUVLength( Vec2::lua_Parse( lua, 2 ) );
			}
			else if( nargs >= 2 ) // setting
			{
				ptr->SetUVLength( Vec2::lua_Read( lua, 2 ) );
			}
			else
			{
				result = Vec2::lua_Write( lua, ptr->GetUVLength() );
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