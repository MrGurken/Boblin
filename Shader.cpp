#include "Shader.h"

Shader::Shader()
{
}

Shader::~Shader()
{
}

bool Shader::Compile( const char* source, GLenum type )
{
	bool result = false;

	GLuint shader = glCreateShader( type );
	if( shader )
	{
		glShaderSource( shader, 1, &source, 0 );
		glCompileShader( shader );

		GLint success;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
		if( success == GL_FALSE )
		{
			char buf[128] = {};
			int len = 128;
			glGetShaderInfoLog( shader, 128, &len, buf );

			printf( "Shader.cpp: %s\n", buf );
		}
		else
		{
			glAttachShader( m_glProgram, shader );
			result = true;

			// TODO: Check if we can delete the shader after we have attached it to the program.
			// If we don't delete the shader, we have a resource leak.
		}
	}

	return result;
}

bool Shader::Link()
{
	bool result = false;
	glLinkProgram( m_glProgram );

	GLint success;
	glGetProgramiv( m_glProgram, GL_LINK_STATUS, &success );
	if( success == GL_FALSE )
	{
		char buf[128] = {};
		int len = 128;
		glGetProgramInfoLog( m_glProgram, 128, &len, buf );

		printf( "Shader.cpp: %s\n", buf );
	}
	else
		result = true;

	return result;
}

void Shader::Bind()
{
	glUseProgram( m_glProgram );
}

bool Shader::AddUniform( const string& uniform )
{
	bool result = false;

	GLuint location = glGetUniformLocation( m_glProgram, uniform.c_str() );
	if( location >= 0 )
	{
		m_mapUniforms.insert( pair<string,GLuint>( uniform, location ) );
		result = true;
	}

	return result;
}

void Shader::SetUniformi( const string& uniform, int value )
{
	map<string,GLuint>::iterator it = m_mapUniforms.find( uniform );
	if( it != m_mapUniforms.end() )
		glUniform1i( it->second, value );
}

void Shader::SetUniformf( const string& uniform, float value )
{
	map<string,GLuint>::iterator it = m_mapUniforms.find( uniform );
	if( it != m_mapUniforms.end() )
		glUniform1f( it->second, value );
}

void Shader::SetUniform( const string& uniform, vec2 value )
{
	map<string,GLuint>::iterator it = m_mapUniforms.find( uniform );
	if( it != m_mapUniforms.end() )
		glUniform2f( it->second, value.x, value.y );
}

void Shader::SetUniform( const string& uniform, vec3 value )
{
	map<string,GLuint>::iterator it = m_mapUniforms.find( uniform );
	if( it != m_mapUniforms.end() )
		glUniform3f( it->second, value.x, value.y, value.z );
}

void Shader::SetUniform( const string& uniform, mat4 value )
{
	map<string,GLuint>::iterator it = m_mapUniforms.find( uniform );
	if( it != m_mapUniforms.end() )
		glUniformMatrix4fv( it->second, 1, GL_FALSE, &value[0][0] );
}