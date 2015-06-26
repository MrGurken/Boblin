#include "GL\glew.h"
#include "SDL.h"
#include "SDL_main.h"
#include "SDL_opengl.h"
#include "lua.hpp"
#include <stdio.h>

#include "mesh.h"
#include "Shader.h"

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
					printf( "main.cpp: OpenGL version = %s\n", glGetString( GL_VERSION ) );

					Vertex vertices[] =
					{
						{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
						{ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
						{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
						{ 1.0f, 1.0f, 0.0f, 0.0f, 0.0f }
					};

					GLuint indices[] =
					{
						0, 1, 2,
						1, 3, 2
					};

					Mesh mesh;
					mesh.AddVertices( vertices, 4, indices, 6 );

					const char* vsource = "#version 450\n"
						"layout (location=0) in vec3 PositionIn;"
						"void main() { gl_Position = vec4( PositionIn, 1.0 ); }";

					const char* fsource = "#version 450\n"
						"out vec4 FragColor;"
						"void main() { FragColor = vec4( 0.0, 0.0, 1.0, 1.0 ); }";

					Shader shader;
					shader.Compile( vsource, GL_VERTEX_SHADER );
					shader.Compile( fsource, GL_FRAGMENT_SHADER );
					shader.Link();

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

						mesh.Render();

						SDL_GL_SwapWindow( window );

						// Adjust time
						SDL_Delay( 100 );
					}
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