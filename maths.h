#ifndef MATHS_H
#define MATHS_H

#include "lua.hpp"
#include "glm.hpp"
#include "quaternion.hpp"
#include "matrix_transform.hpp"
using namespace glm;

namespace Maths
{
    void lua_Register( lua_State* lua );
}

class rect
{
public:
	rect( float value = 0.0f );
	rect( float x, float y, float w, float h );
	rect( const rect& ref );
	~rect();

	rect& operator=( const rect& ref );
	
	void Left( float val );
	void Right( float val );
	void Top( float val );
	void Bottom( float val );

	void TopLeft( vec2 val );
	void TopRight( vec2 val );
	void BottomLeft( vec2 val );
	void BottomRight( vec2 val );

	void Center( vec2 val );

	float Left() const;
	float Right() const;
	float Top() const;
	float Bottom() const;

	vec2 TopLeft() const;
	vec2 TopRight() const;
	vec2 BottomLeft() const;
	vec2 BottomRight() const;

	vec2 Center() const;

	bool Intersect( const rect& ref ) const;
	bool Contains( vec2 point ) const;
    
    static void lua_Register( lua_State* lua );
	static rect lua_Parse( lua_State* lua, int index );
    static rect lua_Read( lua_State* lua, int index );
    static int lua_Write( lua_State* lua, const rect& value );
    static int lua_Center( lua_State* lua );
    static int lua_Intersect( lua_State* lua );
    static int lua_Contains( lua_State* lua );

	float x, y, w, h;
};

namespace Vec2
{
    void lua_Register( lua_State* lua );
	vec2 lua_Parse( lua_State* lua, int index );
    vec2 lua_Read( lua_State* lua, int index );
    int lua_Write( lua_State* lua, const vec2& value );
    int lua_Length( lua_State* lua );
	int lua_Normalize( lua_State* lua );
}

namespace Vec3
{
    void lua_Register( lua_State* lua );
	vec3 lua_Parse( lua_State* lua, int index );
    vec3 lua_Read( lua_State* lua, int index );
    int lua_Write( lua_State* lua, const vec3& value );
    int lua_Length( lua_State* lua );
	int lua_Normalize( lua_State* lua );
}

namespace Vec4
{
	void lua_Register( lua_State* lua );
	vec4 lua_Parse( lua_State* lua, int index );
	vec4 lua_Read( lua_State* lua, int index );
	int lua_Write( lua_State* lua, const vec4& value );
	int lua_Length( lua_State* lua );
	int lua_Normalize( lua_State* lua );
}

namespace Quat
{
	void lua_Register( lua_State* lua );
	quat lua_Parse( lua_State* lua, int index );
	quat lua_Read( lua_State* lua, int index );
	int lua_Write( lua_State* lua, const quat& value );
};

namespace Mat4
{
    void lua_Register( lua_State* lua );
    mat4 lua_Read( lua_State* lua, int index );
    int lua_Write( lua_State* lua, mat4 value );
    int lua_Translation( lua_State* lua );
    int lua_Scale( lua_State* lua );
    int lua_Rotation( lua_State* lua );
    int lua_Orthographic( lua_State* lua );
    int lua_Perspective( lua_State* lua );
}

#endif