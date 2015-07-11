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
	void	SetUniform( const string& uniform, vec4 value );
	void	SetUniform( const string& uniform, mat4 value );

private:
	GLuint	m_glProgram;
	map<string,GLuint> m_mapUniforms;
};

#endif