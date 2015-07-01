#include "Shader.h"

Shader::Shader()
{
	m_glProgram = glCreateProgram();
}

Shader::Shader( const Shader& ref )
	: m_glProgram( ref.m_glProgram ), m_mapUniforms( ref.m_mapUniforms )
{
}

Shader::~Shader()
{
}

bool Shader::Load( const char* filename, GLenum type )
{
    bool result = false;
    
    ifstream stream( filename );
    if( stream )
    {
        stringstream ss;
        ss << stream.rdbuf();
        stream.close();
        
        result = Compile( ss.str().c_str(), type );
    }
    
    return result;
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
			char buf[DEBUG_LOG_SIZE] = {};
			int len = DEBUG_LOG_SIZE;
			glGetShaderInfoLog( shader, DEBUG_LOG_SIZE, &len, buf );

			printf( "Shader.cpp: %s\n", buf );
		}
		else
		{
			glAttachShader( m_glProgram, shader );
			result = true;

			// TODO: Check if we can delete the shader after we have attached it to the program.
			// If we don't delete the shader, we have a resource leak.
			// NOTE: It seems to be working. But make sure.
			glDeleteShader( shader );
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
		char buf[DEBUG_LOG_SIZE] = {};
		int len = DEBUG_LOG_SIZE;
		glGetProgramInfoLog( m_glProgram, DEBUG_LOG_SIZE, &len, buf );

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

void Shader::Destroy()
{
	glDeleteProgram( m_glProgram );
}

bool Shader::AddUniform( const string& uniform )
{
	bool result = false;

	GLint location = glGetUniformLocation( m_glProgram, uniform.c_str() );
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

void Shader::lua_Register( lua_State* lua )
{
    luaL_Reg funcs[] =
    {
        { "Load", lua_Load },
        { "Link", lua_Link },
        { "Bind", lua_Bind },
        { "Destroy", lua_Destroy },
        { "AddUniform", lua_AddUniform },
        { "Int", lua_Int },
        { "Float", lua_Float },
        { "Vec2", lua_Vec2 },
        { "Vec3", lua_Vec3 },
        { "Mat4", lua_Mat4 },
        { NULL, NULL }
    };
    
    luaL_newmetatable( lua, "Shader" );
    luaL_setfuncs( lua, funcs, 0 );
    lua_pushvalue( lua, -1 );
    lua_setfield( lua, -2, "__index" );
    lua_setglobal( lua, "Shader" );
}

Shader* Shader::lua_Read( lua_State* lua, int index )
{
    Shader* result = nullptr;
    if( lua_istable( lua, index ) )
    {
        lua_getfield( lua, index, "__self" );
        result = reinterpret_cast<Shader*>( lua_touserdata( lua, -1 ) );
    }
    
    return result;
}

int Shader::lua_Write( lua_State* lua, Shader* shader )
{
    lua_newtable( lua );
    lua_pushlightuserdata( lua, shader );
    lua_setfield( lua, -2, "__self" );
    luaL_getmetatable( lua, "Shader" );
    lua_setmetatable( lua, -2 );
    return 1;
}

int Shader::lua_Load( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 1 )
    {
        Shader* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            if( nargs >= 3 )
            {
                const char* filename = lua_tostring( lua, 2 );
                int ltype = static_cast<int>( lua_tonumber( lua, 3 ) );
                GLenum gltype = ( ltype == 0 ? GL_VERTEX_SHADER : ( ltype == 1 ? GL_FRAGMENT_SHADER : GL_GEOMETRY_SHADER ) );
                
                lua_pushboolean( lua, ptr->Load( filename, gltype ) );
                result = 1;
            }
        }
    }
    
    return result;
}

int Shader::lua_Link( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 1 )
    {
        Shader* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            lua_pushboolean( lua, ptr->Link() );
            result = 1;
        }
    }
    
    return result;
}

int Shader::lua_Bind( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 1 )
    {
        Shader* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            ptr->Bind();
        }
    }
    
    return result;
}

int Shader::lua_Destroy( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 1 )
    {
        Shader* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            ptr->Destroy();
        }
    }
    
    return result;
}

int Shader::lua_AddUniform( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 2 )
    {
        Shader* ptr = lua_Read( lua, 1 );
        const char* uniform = lua_tostring( lua, 2 );
        
        if( ptr )
        {
            lua_pushboolean( lua, ptr->AddUniform( uniform ) );
            result = 1;
        }
    }
    
    return result;
}

int Shader::lua_Int( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 3 )
    {
        Shader* ptr = lua_Read( lua, 1 );
        const char* uniform = lua_tostring( lua, 2 );
        int value = static_cast<int>( lua_tonumber( lua, 3 ) );
        
        if( ptr )
            ptr->SetUniformi( uniform, value );
    }
    
    return result;
}

int Shader::lua_Float( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 3 )
    {
        Shader* ptr = lua_Read( lua, 1 );
        const char* uniform = lua_tostring( lua, 2 );
        float value = static_cast<float>( lua_tonumber( lua, 3 ) );
        
        if( ptr )
            ptr->SetUniformf( uniform, value );
    }
    
    return result;
}

int Shader::lua_Vec2( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 4 )
    {
        Shader* ptr = lua_Read( lua, 1 );
        const char* uniform = lua_tostring( lua, 2 );
        vec2 v( lua_tonumber( lua, 3 ), lua_tonumber( lua, 4 ) );
        
        if( ptr )
            ptr->SetUniform( uniform, v );
    }
    
    return result;
}

int Shader::lua_Vec3( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 5 )
    {
        Shader* ptr = lua_Read( lua, 1 );
        const char* uniform = lua_tostring( lua, 2 );
        vec3 v( lua_tonumber( lua, 3 ), lua_tonumber( lua, 4 ), lua_tonumber( lua, 5 ) );
        
        if( ptr )
            ptr->SetUniform( uniform, v );
    }
    
    return result;
}

int Shader::lua_Mat4( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs )
    {
        Shader* ptr = lua_Read( lua, 1 );
        const char* uniform = lua_tostring( lua, 2 );
        //mat4 m = lua_ReadMat4( lua, 3 );
        
        // TODO: Complete this
        if( ptr )
            ptr->SetUniform( uniform, mat4() );
    }
    
    return result;
}