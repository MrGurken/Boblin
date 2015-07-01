#include "maths.h"

void Maths::lua_Register( lua_State* lua )
{
    rect::lua_Register( lua );
    Vec2::lua_Register( lua );
    Vec3::lua_Register( lua );
    Mat4::lua_Register( lua );
}

// ***********************************************************************************
// rect
// ***********************************************************************************

rect::rect( float value )
	: x( value ), y( value ), w( value ), h( value )
{
}

rect::rect( float _x, float _y, float _w, float _h )
	: x(_x), y(_y), w(_w), h(_h)
{
}

rect::rect( const rect& ref )
	: x( ref.x ), y( ref.y ), w( ref.w ), h( ref.h )
{
}

rect::~rect()
{
}

rect& rect::operator=( const rect& ref )
{
	x = ref.x;
	y = ref.y;
	w = ref.w;
	h = ref.h;
	return *this;
}

void rect::Left( float val ) { x = val; }
void rect::Right( float val ) { x = val - w; }
void rect::Top( float val ) { y = val; }
void rect::Bottom( float val ) { y = val - h; }
void rect::TopLeft( vec2 val ) { Top( val.y ); Left( val.x ); }
void rect::TopRight( vec2 val ) { Top( val.y ); Right( val.x ); }
void rect::BottomLeft( vec2 val ) { Bottom( val.y ); Left( val.x ); }
void rect::BottomRight( vec2 val ) { Bottom( val.y ); Right( val.x ); }
void rect::Center( vec2 val ) { x = val.x - w*0.5f; y = val.y - h * 0.5f; }

float rect::Left() const { return x; }
float rect::Right() const { return x+w; }
float rect::Top() const { return y; }
float rect::Bottom() const { return y+h; }
vec2 rect::TopLeft() const { return vec2( x, y ); }
vec2 rect::TopRight() const { return vec2( x+w, y ); }
vec2 rect::BottomLeft() const { return vec2( x, y+h ); }
vec2 rect::BottomRight() const { return vec2( x+w, y+h ); }
vec2 rect::Center() const { return vec2( x+w*0.5f, y+h*0.5f ); }

bool rect::Intersect( const rect& ref ) const
{
	if( Left() > ref.Right() ) return false;
	if( Right() < ref.Left() ) return false;
	if( Top() > ref.Bottom() ) return false;
	if( Bottom() < ref.Top() ) return false;
	return true;
}

bool rect::Contains( vec2 point ) const
{
	return ( point.x >= Left() && point.x <= Right() && point.y >= Top() && point.y <= Bottom() );
}

void rect::lua_Register( lua_State* lua )
{
    luaL_Reg funcs[] =
    {
        { "Center", lua_Center },
        { "Intersect", lua_Intersect },
        { "Contains", lua_Contains },
        { NULL, NULL }
    };
    
    luaL_newmetatable( lua, "Rect" );
    luaL_setfuncs( lua, funcs, 0 );
    lua_pushvalue( lua, -1 );
    lua_setfield( lua, -2, "__index" );
    lua_setglobal( lua, "Rect" );
}

rect rect::lua_Read( lua_State* lua, int index )
{
    lua_getfield( lua, index, "x" );
    lua_getfield( lua, index, "y" );
    lua_getfield( lua, index, "w" );
    lua_getfield( lua, index, "h" );
    return rect( static_cast<float>( lua_tonumber( lua, -1 ) ), static_cast<float>( lua_tonumber( lua, -1 ) ),
                static_cast<float>( lua_tonumber( lua, -1 ) ), static_cast<float>( lua_tonumber( lua, -1 ) ) );
}

int rect::lua_Write( lua_State* lua, rect value )
{
    lua_newtable( lua );
    lua_pushnumber( lua, value.x );
    lua_setfield( lua, -2, "x" );
    lua_pushnumber( lua, value.y );
    lua_setfield( lua, -2, "y" );
    lua_pushnumber( lua, value.w );
    lua_setfield( lua, -2, "w" );
    lua_pushnumber( lua, value.h );
    lua_setfield( lua, -2, "h" );
    return 1;
}

int rect::lua_Center( lua_State* lua )
{
    int result = 0;
    
    if( lua_gettop( lua ) >= 1 )
    {
        rect r = lua_Read( lua, 1 );
        result = Vec2::lua_Write( lua, r.Center() );
    }
    
    return result;
}

int rect::lua_Intersect( lua_State* lua )
{
    int result = 0;
    
    if( lua_gettop( lua ) >= 2 )
    {
        rect a = lua_Read( lua, 1 );
        rect b = lua_Read( lua, 2 );
        lua_pushboolean( lua, a.Intersect( b ) );
        result = 1;
    }
    
    return result;
}

int rect::lua_Contains( lua_State* lua )
{
    int result = 0;
    
    if( lua_gettop( lua ) >= 2 )
    {
        rect r = lua_Read( lua, 1 );
        vec2 v = Vec2::lua_Read( lua, 2 );
        lua_pushboolean( lua, r.Contains( v ) );
        result = 1;
    }
    
    return result;
}

// ***********************************************************************************
// VEC 2
// ***********************************************************************************

void Vec2::lua_Register( lua_State* lua )
{
    luaL_Reg funcs[] =
    {
        { "Length", lua_Length },
        { NULL, NULL }
    };
    
    luaL_newmetatable( lua, "Vec2" );
    luaL_setfuncs( lua, funcs, 0 );
    lua_pushvalue( lua, -1 );
    lua_setfield( lua, -2, "__index" );
    lua_setglobal( lua, "Vec2" );
}

vec2 Vec2::lua_Read( lua_State* lua, int index )
{
    lua_getfield( lua, index, "y" );
    lua_getfield( lua, index, "x" );
    return vec2( static_cast<float>( lua_tonumber( lua, -1 ) ), static_cast<float>( lua_tonumber( lua, -1 ) ) );
}

int Vec2::lua_Write( lua_State* lua, vec2 value )
{
    lua_newtable( lua );
    lua_pushnumber( lua, value.x );
    lua_setfield( lua, -2, "x" );
    lua_pushnumber( lua, value.y );
    lua_setfield( lua, -2, "y" );
    luaL_getmetatable( lua, "Vec2" );
    lua_setmetatable( lua, -2 );
    return 1;
}

int Vec2::lua_Length( lua_State *lua )
{
    int result = 0;
    if( lua_gettop( lua ) >= 1 )
    {
        vec2 v = lua_Read( lua, 1 );
        lua_pushnumber( lua, v.length() );
        result = 1;
    }
    
    return result;
}

// ***********************************************************************************
// VEC 3
// ***********************************************************************************

void Vec3::lua_Register( lua_State* lua )
{
    luaL_Reg funcs[] =
    {
        { "Length", lua_Length },
        { NULL, NULL }
    };
    
    luaL_newmetatable( lua, "Vec3" );
    luaL_setfuncs( lua, funcs, 0);
    lua_pushvalue( lua, -1 );
    lua_setfield( lua, -2, "__index" );
    lua_setglobal( lua, "Vec3" );
}

vec3 Vec3::lua_Read( lua_State* lua, int index )
{
    lua_getfield( lua, index, "z" );
    lua_getfield( lua, index, "y" );
    lua_getfield( lua, index, "x" );
    return vec3( static_cast<float>( lua_tonumber( lua, -1 ) ), static_cast<float>( lua_tonumber( lua, -1 ) ), static_cast<float>( lua_tonumber( lua, -1 ) ) );
}

int Vec3::lua_Write( lua_State* lua, vec3 value )
{
    lua_newtable( lua );
    lua_pushnumber( lua, value.x );
    lua_setfield( lua, -2, "x" );
    lua_pushnumber( lua, value.y );
    lua_setfield( lua, -2, "y" );
    lua_pushnumber( lua, value.z );
    lua_setfield( lua, -2, "z" );
    luaL_getmetatable( lua, "Vec3" );
    lua_setmetatable( lua, -2 );
    return 1;
}

int Vec3::lua_Length( lua_State* lua )
{
    int result = 0;
    if( lua_gettop( lua ) >= 1 )
    {
        vec3 v = lua_Read( lua, 1 );
        lua_pushnumber( lua, v.length() );
        result = 1;
    }
    
    return result;
}

// ***********************************************************************************
// MAT4
// ***********************************************************************************

void Mat4::lua_Register( lua_State* lua )
{
    luaL_Reg funcs[] =
    {
        { "Translation", lua_Translation },
        { "Scale", lua_Scale },
        { "Rotation", lua_Rotation },
        { "Orthographic", lua_Orthographic },
        { "Perspective", lua_Perspective },
        { NULL, NULL }
    };
    
    luaL_newmetatable( lua, "Mat4" );
    luaL_setfuncs( lua, funcs, 0 );
    lua_pushvalue( lua, -1 );
    lua_setfield( lua, -2, "__index" );
    lua_setglobal( lua, "Mat4" );
}

mat4 Mat4::lua_Read( lua_State* lua, int index )
{
    mat4 result;
    for( int i=0; i<4; i++ )
    {
        for( int j=0; j<4; j++ )
        {
            lua_rawgeti( lua, index, i*4+j );
            result[i][j] = static_cast<float>( lua_tonumber( lua, -1 ) );
        }
    }
    
    return result;
}

int Mat4::lua_Write( lua_State* lua, mat4 value )
{
    lua_newtable( lua );
    for( int i=0; i<4; i++ )
    {
        for( int j=0; j<4; j++ )
        {
            lua_pushnumber( lua, value[i][j] );
            lua_rawseti( lua, -2, i*4+j );
        }
    }
    luaL_getmetatable( lua, "Mat4" );
    lua_setmetatable( lua, -2 );
    return 1;
}

int Mat4::lua_Translation( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 3 ) // componentwise
    {
        vec3 v( static_cast<float>( lua_tonumber( lua, 1 ) ), static_cast<float>( lua_tonumber( lua, 2 ) ), static_cast<float>( lua_tonumber( lua, 3 ) ) );
        result = Mat4::lua_Write( lua, translate( mat4(), v ) );
    }
    else if( nargs >= 1 )
    {
        vec3 v = Vec3::lua_Read( lua, 1 );
        result = Mat4::lua_Write( lua, translate( mat4(), v ) );
    }
    
    return result;
}

int Mat4::lua_Scale( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 3 ) // componentwise
    {
        vec3 v( static_cast<float>( lua_tonumber( lua, 1 ) ), static_cast<float>( lua_tonumber( lua, 2 ) ), static_cast<float>( lua_tonumber( lua, 3 ) ) );
        result = Mat4::lua_Write( lua, scale( mat4(), v ) );
    }
    else if( nargs >= 1 )
    {
        vec3 v = Vec3::lua_Read( lua, 1 );
        result = Mat4::lua_Write( lua, scale( mat4(), v ) );
    }
    
    return result;
}

int Mat4::lua_Rotation( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 4 ) // componentwise
    {
        vec3 v( static_cast<float>( lua_tonumber( lua, 1 ) ), static_cast<float>( lua_tonumber( lua, 2 ) ), static_cast<float>( lua_tonumber( lua, 3 ) ) );
        float angle = lua_tonumber( lua, 4 );
        result = Mat4::lua_Write( lua, rotate( mat4(), angle, v ) );
    }
    else if( nargs >= 2 )
    {
        vec3 v = Vec3::lua_Read( lua, 1 );
        float angle = lua_tonumber( lua, 2 );
        result = Mat4::lua_Write( lua, rotate( mat4(), angle, v ) );
    }
    
    return result;
}

int Mat4::lua_Orthographic( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 6 )
    {
        float left = static_cast<float>( lua_tonumber( lua, 1 ) );
        float right = static_cast<float>( lua_tonumber( lua, 2 ));
        float top = static_cast<float>( lua_tonumber( lua, 3 ));
        float bottom = static_cast<float>( lua_tonumber( lua, 4 ) );
        float nearplane = static_cast<float>( lua_tonumber( lua, 5 ));
        float farplane = static_cast<float>( lua_tonumber( lua, 6 ) );
        result = lua_Write( lua, ortho( left, right, bottom, top, nearplane, farplane ) );
    }
    
    return result;
}

int Mat4::lua_Perspective( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 4 )
    {
        float fov = static_cast<float>( lua_tonumber( lua, 1 ) );
        float aspect = static_cast<float>( lua_tonumber( lua, 2 ) );
        float nearplane = static_cast<float>( lua_tonumber( lua, 3 ) );
        float farplane = static_cast<float>( lua_tonumber( lua, 4 ) );
        result = lua_Write( lua, perspective( fov, aspect, nearplane, farplane ) );
    }
    
    return result;
}