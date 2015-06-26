#ifndef SHADER_H
#define SHADER_H

#include "GL\glew.h"
#include "glm.hpp"
using namespace glm;
#include <string>
using std::string;
#include <map>
using std::map;
using std::pair;

class Shader
{
public:
	Shader();
	virtual ~Shader();

	bool	Compile( const char* source, GLenum type );
	bool	Link();
	void	Bind();

	bool	AddUniform( const string& uniform );
	void	SetUniformi( const string& uniform, int value );
	void	SetUniformf( const string& uniform, float value );
	void	SetUniform( const string& uniform, vec2 value );
	void	SetUniform( const string& uniform, vec3 value );
	void	SetUniform( const string& uniform, mat4 value );

private:
	GLuint m_glProgram;
	map<string,GLuint> m_mapUniforms;
};

#endif