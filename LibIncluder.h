#ifdef WIN32
#include "GL\glew.h"
#include "SDL.h"
#include "SDL_main.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#else
#include <OpenGL/gl3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif