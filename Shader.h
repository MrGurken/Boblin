#ifndef SHADER_H
#define SHADER_H

#include "Assets.h"
#include "GL\glew.h"
#include "glm.hpp"
#include "gtc\matrix_transform.hpp"
#include "gtx\transform.hpp"
using namespace glm;
#include <string>
using std::string;
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

private:
	GLuint m_glProgram;
	map<string,GLuint> m_mapUniforms;
};

#endif