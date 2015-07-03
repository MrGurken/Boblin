#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "LibIncluder.h"

#include "lua.hpp"
#include "maths.h"
#include <vector>
using std::vector;

#include "Texture.h"
#include "mesh.h"
#include "Shader.h"

#define LDEC( _name ) static int lua_ ## _name( lua_State* )
#define LIMP( _name ) int GameObject::lua_ ## _name( lua_State* lua )

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Update();
	void Render( Shader* shader );

	bool Collides( const GameObject& ref );

	static void UpdateAll();
	static void RenderAll( Shader* shader );

	void SetCollisionBounds( rect bounds );
	void SetRenderBounds( rect bounds );
	void SetPosition( vec2 position );
	void SetVelocity( vec2 velocity );
	void SetFriction( vec2 friction );
	void SetColor( vec4 color );
	void SetTexture( Texture* texture );
	void SetAlive( bool alive );

	rect GetCollisionBounds() const;
	rect GetRenderBounds() const;
	vec2 GetPosition() const;
	vec2 GetVelocity() const;
	vec2 GetFriction() const;
	vec4 GetColor() const;
	Texture* GetTexture() const;
	bool GetAlive() const;

	static void lua_Register( lua_State* lua );
	static GameObject* lua_Read(lua_State* lua, int index);
	static int lua_Write(lua_State* lua, GameObject* object);
	LDEC( Create );
	LDEC( Destroy );
	LDEC( CollisionBounds );
	LDEC( RenderBounds );
	LDEC( Position );
	LDEC( Velocity );
	LDEC( Friction );
	LDEC( Color );
	LDEC( Texture );
	LDEC( Alive );
	LDEC( Collides );

private:
	rect m_collisionBounds;
	rect m_renderBounds;
	vec2 m_position;
	vec2 m_velocity;
	vec2 m_friction;
	vec4 m_color;
	Texture* m_pTexture;
	bool m_bAlive;

	static vector<GameObject*> s_vecObjects;
	static Mesh* quad;
};

#endif