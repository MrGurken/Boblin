#include "Texture.h"

Texture::Texture()
	: m_glID( 0 ), m_iWidth( 0 ), m_iHeight( 0 )
{
}

Texture::Texture( const Texture& ref )
	: m_glID( ref.m_glID ), m_iWidth( ref.m_iWidth ), m_iHeight( ref.m_iHeight )
{
}

Texture::~Texture()
{
}

//bool Texture::Load( const string& filename )
bool Texture::Load( const AssetInfo* info )
{
	bool result = false;

	//SDL_Surface* img = IMG_Load( filename.c_str() );
	SDL_Surface* img = IMG_Load( info->filename );
	if( img )
	{
		glGenTextures( 1, &m_glID );
		glBindTexture( GL_TEXTURE_2D, m_glID );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

		GLenum mode = ( img->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, mode, GL_UNSIGNED_BYTE, img->pixels );

		m_iWidth = img->w;
		m_iHeight = img->h;

		SDL_FreeSurface( img );
		result = true;
	}

	return result;
}

void Texture::Unload()
{
	if( m_glID != 0 )
		glDeleteTextures( 1, &m_glID );

	m_glID = 0;
	m_iWidth = m_iHeight = 0;
}

void Texture::Bind( GLenum slot )
{
	glActiveTexture( slot );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_glID );
}

int Texture::GetWidth() const { return m_iWidth; }
int Texture::GetHeight() const { return m_iHeight; }

void Texture::lua_Register( lua_State* lua )
{
	luaL_Reg funcs[] =
	{
		{ "Load", lua_Load },
		{ NULL, NULL }
	};

	luaL_newmetatable( lua, "Texture" );
	luaL_setfuncs( lua, funcs, 0 );
	lua_pushvalue( lua, -1 );
	lua_setfield( lua, -2, "__index" );
	lua_setglobal( lua, "Texture" );
}

Texture* Texture::lua_Read( lua_State* lua, int index )
{
	Texture* result = nullptr;
	if( lua_istable( lua, index ) )
	{
		lua_getfield( lua, index, "__self" );
		result = reinterpret_cast<Texture*>( lua_touserdata( lua, -1 ) );
	}

	return result;
}

int Texture::lua_Write( lua_State* lua, Texture* texture )
{
	lua_newtable( lua );
	lua_pushlightuserdata( lua, texture );
	lua_setfield( lua, -2, "__self" );
	lua_pushnumber( lua, texture->GetWidth() );
	lua_setfield( lua, -2, "width" );
	lua_pushnumber( lua, texture->GetHeight() );
	lua_setfield( lua, -2, "height" );
	luaL_getmetatable( lua, "Texture" );
	lua_setmetatable( lua, -2 );
	return 1;
}

int Texture::lua_Load( lua_State* lua )
{
	int result = 0;

	if( lua_gettop( lua ) >= 1 )
	{
		const char* path = lua_tostring( lua, 1 );
		AssetInfo info = { path };
		Texture* t = Assets::Instance().Load<Texture>( &info );

		if( t )
		{
			result = lua_Write( lua, t );
		}
	}

	return result;
}