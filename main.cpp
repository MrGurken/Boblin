#include "lua.hpp"
#include <stdio.h>

#include "mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Camera.h"
#include "Script.h"
#include "Input.h"

int main( int argc, char* argv[] )
{
	int result = 0;

	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
	{
		printf( "main.cpp: Failed to initialize SDL.\n" );
		result = -1;
	}
	else
	{
        // TODO: Does this work on win32?
#ifndef WIN32
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
#endif
		// TODO: Enable config file for window size and title
		SDL_Window* window = SDL_CreateWindow( "Boblin", 1080, 128, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		if( window )
		{
			SDL_GLContext glContext = SDL_GL_CreateContext(window);
			if( glContext )
			{
#ifdef WIN32
				glewExperimental = GL_TRUE;
				GLenum status = glewInit();
				if( status != GLEW_OK )
				{
					printf( "main.cpp: Failed to initialize GLEW.\n" );
					result = -1;
				}
				else
#endif
				{
					// TODO: Remove this
					glDisable( GL_CULL_FACE );

					const char* vsource = "#version 330\n"
						"layout (location=0) in vec3 PositionIn;"
						"layout (location=1) in vec2 UVIn;"
						"out vec2 UV0;"
						"uniform mat4 ModelMatrix;"
						"uniform mat4 ViewMatrix;"
						"uniform mat4 ProjectionMatrix;"
						"void main() { gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4( PositionIn, 1.0 ); UV0 = UVIn; }";

					const char* fsource = "#version 330\n"
						"in vec2 UV0;"
						"out vec4 FragColor;"
						"uniform sampler2D diffuseMap;"
						"void main() { FragColor = texture( diffuseMap, UV0 ); }";

					Shader shader;
					shader.Compile( vsource, GL_VERTEX_SHADER );
					shader.Compile( fsource, GL_FRAGMENT_SHADER );
					shader.Link();

					shader.AddUniform( "ModelMatrix" );
					shader.AddUniform( "ViewMatrix" );
					shader.AddUniform( "ProjectionMatrix" );

					// TODO: Instead of assuming that the function we are looking for is
					// called 'main', let Lua register it's main function with us.
					int mainFunctionRef = -1;
					Script lua;
					if( lua.Run( "./res/scripts/main.lua" ) )
					{
						lua_getglobal( lua, "main" );
						mainFunctionRef = luaL_ref( lua, LUA_REGISTRYINDEX );
					}

					Input::Instance().SetWindow( window );

					bool validLua = true;
					bool running = true;
					while( running )
					{
						running = Input::Instance().Update();
						if( Input::Instance().KeyDown( SDLK_ESCAPE ) )
							running = false;

						// Update
						if( validLua )
						{
							lua_rawgeti( lua, LUA_REGISTRYINDEX, mainFunctionRef );
							if( lua_pcall( lua, 0, 0, 0 ) )
							{
								printf( "Lua error: %s\n", lua_tostring( lua, -1 ) );
								validLua = false;
							}
						}

						if( lua.Hotload() )
							validLua = true;

						// Render
						glClearColor( 1.0f, 0.0f, 0.0f, 0.0f );
						glClear( GL_COLOR_BUFFER_BIT );

						shader.Bind();
						//texture->Bind();

						Camera* camera = Camera::GetActive();
						if( camera )
						{
							shader.SetUniform( "ViewMatrix", camera->GetView() );
							shader.SetUniform( "ProjectionMatrix", camera->GetProjection() );
						}

						GameObject::RenderAll( &shader );

						SDL_GL_SwapWindow( window );

						// Adjust time
						SDL_Delay( 100 );
					}

					luaL_unref( lua, LUA_REGISTRYINDEX, mainFunctionRef );
				}

				SDL_GL_DeleteContext( glContext );
			}
			else
			{
				printf( "main.cpp: Failed to create OpenGL context." );
				result = -1;
			}

			SDL_DestroyWindow( window );
		}
		else
		{
			printf( "main.cpp: Failed to create window.\n" );
			result = -1;
		}
	}

	return result;
}