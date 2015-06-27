#include "GL\glew.h"
#include "SDL.h"
#include "SDL_main.h"
#include "SDL_opengl.h"
#include "lua.hpp"
#include <stdio.h>

#include "mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"

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
		// TODO: Enable config file for window size and title
		SDL_Window* window = SDL_CreateWindow( "Boblin", 128, 128, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		if( window )
		{
			SDL_GLContext glContext = SDL_GL_CreateContext(window);
			if( glContext )
			{
				glewExperimental = GL_TRUE;
				GLenum status = glewInit();
				if( status != GLEW_OK )
				{
					printf( "main.cpp: Failed to initialize GLEW.\n" );
					result = -1;
				}
				else
				{
					const char* vsource = "#version 450\n"
						"layout (location=0) in vec3 PositionIn;"
						"layout (location=1) in vec2 UVIn;"
						"out vec2 UV0;"
						"void main() { gl_Position = vec4( PositionIn, 1.0 ); UV0 = UVIn; }";

					const char* fsource = "#version 450\n"
						"in vec2 UV0;"
						"out vec4 FragColor;"
						"uniform sampler2D diffuseMap;"
						"void main() { FragColor = texture( diffuseMap, UV0 ); }";

					Shader shader;
					shader.Compile( vsource, GL_VERTEX_SHADER );
					shader.Compile( fsource, GL_FRAGMENT_SHADER );
					shader.Link();

					Texture* texture = Assets::Instance().Load<Texture>( "./res/textures/Koala.jpg" );
					if( texture == nullptr )
					{
						printf( "main.cpp: Failed to load texture.\n" );
					}

					lua_State* lua = luaL_newstate();
					luaL_openlibs( lua );
					GameObject::lua_Register( lua );
					
					luaL_loadfile( lua, "./res/scripts/main.lua" );
					if( lua_pcall( lua, 0, 0, 0 ) )
					{
						printf( "Lua error: %s\n", lua_tostring( lua, -1 ) );
					}

					bool running = true;
					SDL_Event e;
					while( running )
					{
						while( SDL_PollEvent( &e ) )
						{
							if( e.type == SDL_QUIT )
								running = false;
							else if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE )
								running = false;
						}

						// Update

						// Render
						glClearColor( 1.0f, 0.0f, 0.0f, 0.0f );
						glClear( GL_COLOR_BUFFER_BIT );

						shader.Bind();
						texture->Bind();

						GameObject::RenderAll( &shader );

						SDL_GL_SwapWindow( window );

						// Adjust time
						SDL_Delay( 100 );
					}

					lua_close( lua );
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