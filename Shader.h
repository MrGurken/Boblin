#ifndef SHADER_H
#define SHADER_H

#include "LibIncluder.h"
#include "lua.hpp"
#include "glm.hpp"
#include "matrix_transform.hpp"
#include "transform.hpp"
using namespace glm;

#include "maths.h"
#include "Assets.h"
#include <fstream>
using std::ifstream;
#include <string>
using std::string;
#include <sstream>
using std::stringstream;
#include <map>
using std::map;
using std::pair;

#define DEBUG_LOG_SIZE 1024

class Shader
{
public:
	Shader();
	Shader( const Shader& ref );
	virtual ~Shader();
	
    bool    Load( const char* filename, GLenum type );
	bool	Compile( const char* source, GLenum type );
	bool	Link();
	void	Bind();
	void	Destroy();

	bool	AddUniform( const string& uniform );
	void	SetUniformi( const string& uniform, int value );
	void	SetUniformf( const string& uniform, float value );
	void	SetUniform( const string& uniform, vec2 value );
	void	SetUniform( const string& uniform, vec3 value );
	void	SetUniform( const string& uniform, mat4 value );
    
    static void lua_Register( lua_State* lua );
    static Shader* lua_Read( lua_State* lua, int index );
    static int lua_Write( lua_State* lua, Shader* shader );
    static int lua_Load( lua_State* lua );
    static int lua_Link( lua_State* lua );
    static int lua_Bind( lua_State* lua );
    static int lua_Destroy( lua_State* lua );
    static int lua_AddUniform( lua_State* lua );
    static int lua_Int( lua_State* lua );
    static int lua_Float( lua_State* lua );
    static int lua_Vec2( lua_State* lua );
    static int lua_Vec3( lua_State* lua );
    static int lua_Mat4( lua_State* lua );

private:
	GLuint m_glProgram;
	map<string,GLuint> m_mapUniforms;
};

#endif