#include "Sound.h"

Sound::Sound() : m_sdlChunk( nullptr ), m_iChannel( -1 )
{
    
}

Sound::~Sound()
{
    Unload();
}

bool Sound::Load( const string& filename )
{
    bool result = false;
    
    m_sdlChunk = Mix_LoadWAV( filename.c_str() );
    if( m_sdlChunk )
        result = true;
    
    return result;
}

void Sound::Unload()
{
    if( m_sdlChunk )
    {
        Mix_FreeChunk( m_sdlChunk );
        m_sdlChunk = nullptr;
    }
    
    m_iChannel = -1;
}

void Sound::Play( int channel )
{
    Mix_PlayChannel( channel, m_sdlChunk, 0 );
    m_iChannel = channel;
}

void Sound::Pause()
{
    if( Mix_Playing( m_iChannel ) )
        Mix_Pause( m_iChannel );
}

void Sound::Stop()
{
    Mix_HaltChannel( m_iChannel );
}

void Sound::lua_Register( lua_State* lua )
{
    luaL_Reg funcs[] =
    {
        { "Play", lua_Play },
        { "Pause", lua_Pause },
        { "Stop", lua_Stop },
        { NULL, NULL }
    };
    
    luaL_newmetatable( lua, "Sound" );
    luaL_setfuncs( lua, funcs, 0 );
    lua_pushvalue( lua, -1 );
    lua_setfield( lua, -2, "__index" );
    lua_setglobal( lua, "Sound" );
    
    lua_register( lua, "LoadSound", lua_Load );
    
    Music::lua_Register( lua );
}

Sound* Sound::lua_Read( lua_State* lua, int index )
{
    Sound* result = nullptr;
    
    if( lua_istable( lua, index ) )
    {
        lua_getfield( lua, index, "__self" );
        result = reinterpret_cast<Sound*>( lua_touserdata( lua, -1 ) );
    }
    
    return result;
}

int Sound::lua_Write( lua_State* lua, Sound* sound )
{
    lua_newtable( lua );
    lua_pushlightuserdata( lua, sound );
    lua_setfield( lua, -2, "__self" );
    luaL_getmetatable( lua, "Sound" );
    lua_setmetatable( lua, -2 );
    return 1;
}

int Sound::lua_Load( lua_State* lua )
{
    int result = 0;
    
    if( lua_gettop( lua ) >= 1 )
    {
        const char* filename = lua_tostring( lua, 1 );
        Sound* ptr = Assets::Instance().Load<Sound>( filename );
        if( ptr )
        {
            result = lua_Write( lua, ptr );
        }
    }
    
    return result;
}

int Sound::lua_Play( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 1 )
    {
        Sound* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            int channel = -1;
            if( nargs >= 2 )
                channel = static_cast<int>( lua_tonumber( lua, 2 ) );
            
            ptr->Play( channel );
        }
    }
    
    return result;
}

int Sound::lua_Pause( lua_State* lua )
{
    int result = 0;
    
    if( lua_gettop( lua ) >= 1 )
    {
        Sound* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            ptr->Pause();
        }
    }
    
    return result;
}

int Sound::lua_Stop( lua_State* lua )
{
    int result = 0;
    
    if( lua_gettop( lua ) >= 1 )
    {
        Sound* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            ptr->Stop();
        }
    }
    
    return result;
}

// ***********************************************************************
// Music
// ***********************************************************************

Music::Music() : m_sdlMusic( nullptr ), m_iChannel( -1 )
{
    
}

Music::~Music()
{
    Unload();
}

bool Music::Load( const string& filename )
{
    bool result = false;
    
    m_sdlMusic = Mix_LoadMUS( filename.c_str() );
    if( m_sdlMusic )
        result = true;
    
    return result;
}

void Music::Unload()
{
    if( m_sdlMusic )
    {
        Mix_FreeMusic( m_sdlMusic );
        m_sdlMusic = nullptr;
    }
    
    m_iChannel = -1;
}

void Music::Play( int channel )
{
    if( m_sdlMusic )
    {
        Mix_PlayMusic( m_sdlMusic, -1 );
    }
}

void Music::Pause()
{
    if( Mix_PlayingMusic() )
        Mix_PauseMusic();
}

void Music::Stop()
{
    Mix_HaltMusic();
}

void Music::lua_Register( lua_State* lua )
{
    luaL_Reg funcs[] =
    {
        { "Play", lua_Play },
        { "Pause", lua_Pause },
        { "Stop", lua_Stop },
        { NULL, NULL }
    };
    
    luaL_newmetatable( lua, "Music" );
    luaL_setfuncs( lua, funcs, 0 );
    lua_pushvalue( lua, -1 );
    lua_setfield( lua, -2, "__index" );
    lua_setglobal( lua, "Music" );
    
    lua_register( lua, "LoadMusic", lua_Load );
}

Music* Music::lua_Read( lua_State* lua, int index )
{
    Music* result = nullptr;
    
    if( lua_istable( lua, index ) )
    {
        lua_getfield( lua, index, "__self" );
        result = reinterpret_cast<Music*>( lua_touserdata( lua, -1 ) );
    }
    
    return result;
}

int Music::lua_Write( lua_State* lua, Music* music )
{
    lua_newtable( lua );
    lua_pushlightuserdata( lua, music );
    lua_setfield( lua, -2, "__self" );
    luaL_getmetatable( lua, "Music" );
    lua_setmetatable( lua, -2 );
    return 1;
}

int Music::lua_Load( lua_State* lua )
{
    int result = 0;
    
    if( lua_gettop( lua ) >= 1 )
    {
        const char* filename = lua_tostring( lua, 1 );
        Music* ptr = Assets::Instance().Load<Music>( filename );
        if( ptr )
        {
            result = lua_Write( lua, ptr );
        }
    }
    
    return result;
}

int Music::lua_Play( lua_State* lua )
{
    int result = 0;
    
    int nargs = lua_gettop( lua );
    if( nargs >= 1 )
    {
        Music* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            int channel = -1;
            if( nargs >= 2 )
                channel = static_cast<int>( lua_tonumber( lua, 2 ) );
            
            ptr->Play( channel );
        }
    }
    
    return result;
}

int Music::lua_Pause( lua_State* lua )
{
    int result = 0;
    
    if( lua_gettop( lua ) >= 1 )
    {
        Music* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            ptr->Pause();
        }
    }
    
    return result;
}

int Music::lua_Stop( lua_State* lua )
{
    int result = 0;
    
    if( lua_gettop( lua ) >= 1 )
    {
        Music* ptr = lua_Read( lua, 1 );
        if( ptr )
        {
            ptr->Stop();
        }
    }
    
    return result;
}