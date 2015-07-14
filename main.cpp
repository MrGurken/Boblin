#include "lua.hpp"
#include <stdio.h>
#include "LibIncluder.h"

#include "mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Camera.h"
#include "Script.h"
#include "Input.h"
#include "Sound.h"
#include "Config.h"
#include "Font.h"
#include "Text.h"

int main( int argc, char* argv[] )
{
	int result = 0;

	if( !Config::Instance().Load( "./config.txt" ) )
	{
		printf( "main.cpp: Failed to load configuration file.\n" );
		return -1;
	}

	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 || TTF_Init() < 0 )
	{
		printf( "main.cpp: Failed to initialize SDL.\n" );
		result = -1;
	}
	else
	{
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

		SDL_Window* window = SDL_CreateWindow( Config::Instance().GetTitle().c_str(),
											   Config::Instance().GetStartX(),
											   Config::Instance().GetStartY(),
											   Config::Instance().GetWidth(),
											   Config::Instance().GetHeight(),
											   SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

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
					glEnable( GL_BLEND );
					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
					glClearColor( 1.0f, 0.0f, 0.0f, 0.0f );

					const char* vsource = "#version 330\n"
						"layout (location=0) in vec3 PositionIn;"
						"layout (location=1) in vec2 UVIn;"
						"out vec2 UV0;"
						"uniform mat4 ModelMatrix;"
						"uniform mat4 ViewMatrix;"
						"uniform mat4 ProjectionMatrix;"
						"uniform vec2 UVMin;"
						"uniform vec2 UVLength;"
						"void main() { gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4( PositionIn, 1.0 ); UV0 = UVMin + ( UVIn * UVLength ); }";

					const char* fsource = "#version 330\n"
						"in vec2 UV0;"
						"out vec4 FragColor;"
						"uniform vec4 Color;"
						"uniform sampler2D DiffuseMap;"
						"void main() { FragColor = texture( DiffuseMap, UV0 ) * Color; }";

					Shader shader;
					shader.Compile( vsource, GL_VERTEX_SHADER );
					shader.Compile( fsource, GL_FRAGMENT_SHADER );
					shader.Link();

					shader.AddUniform( "ModelMatrix" );
					shader.AddUniform( "ViewMatrix" );
					shader.AddUniform( "ProjectionMatrix" );
					shader.AddUniform( "UVMin" );
					shader.AddUniform( "UVLength" );
					shader.AddUniform( "Color" );

					string mainScript = Config::Instance().GetScriptFolder() + "/main.lua";
					Runtime::Instance().Run( mainScript );

					Input::Instance().SetWindow( window );

					const unsigned int targetTicks = 1000 / Config::Instance().GetFPS();

					//Input::Instance().UseGamepad( 0, true );
					while( Runtime::Instance().GetRunning() )
					{
						unsigned int startTick = SDL_GetTicks();

						if( !Input::Instance().Update() )
							Runtime::Instance().Quit();

						// Update
						if( Config::Instance().GetDebugMode() )
							Runtime::Instance().Hotload();
						Runtime::Instance().Update();

						GameObject::UpdateAll();

						// Render
						glClear( GL_COLOR_BUFFER_BIT );

						shader.Bind();

						Camera* camera = Camera::GetActive();
						if( camera )
						{
							shader.SetUniform( "ViewMatrix", camera->GetView() );
							shader.SetUniform( "ProjectionMatrix", camera->GetProjection() );
						}

						GameObject::RenderAll( &shader );
						Text::RenderAll( &shader );

						/*if( Input::Instance().GPConnected( 0 ) )
							if( Input::Instance().GPPressed( 0, A ) )
								printf("HAHAHA\n");*/

						SDL_GL_SwapWindow( window );

						// Adjust time
						unsigned int tickDif = SDL_GetTicks() - startTick;
						if( tickDif < targetTicks )
							SDL_Delay( targetTicks - tickDif );
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

		TTF_Quit();
		SDL_Quit();
	}

	return result;
}