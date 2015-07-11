#ifndef SOUND_H
#define SOUND_H

#include "LibIncluder.h"
#include "lua.hpp"
#include "Assets.h"

class Sound : public Asset
{
public:
    Sound();
    virtual ~Sound();
    
    //bool    Load( const string& filename );
	bool	Load( const AssetInfo* info );
    void    Unload();
    
    void    Play( int channel = -1 );
    void    Pause();
    void    Stop();
    
    static void lua_Register( lua_State* lua );
    static Sound* lua_Read( lua_State* lua, int index );
    static int lua_Write( lua_State* lua, Sound* sound );
    static int lua_Load( lua_State* lua );
    static int lua_Play( lua_State* lua );
    static int lua_Pause( lua_State* lua );
    static int lua_Stop( lua_State* lua );
    
private:
    Mix_Chunk*  m_sdlChunk;
    int         m_iChannel;
    bool        m_bPlaying;
};

class Music : public Asset
{
public:
    Music();
    virtual ~Music();
    
    //bool        Load( const string& filename );
	bool		Load( const AssetInfo* info );
    void        Unload();
    
    void        Play( int channel = -1 );
    void        Pause();
    void        Stop();
    
    static void lua_Register( lua_State* lua );
    static Music* lua_Read( lua_State* lua, int index );
    static int lua_Write( lua_State* lua, Music* music );
    static int lua_Load( lua_State* lua );
    static int lua_Play( lua_State* lua );
    static int lua_Pause( lua_State* lua );
    static int lua_Stop( lua_State* lua );
    
private:
    Mix_Music*  m_sdlMusic;
    int         m_iChannel;
    bool        m_bPlaying;
};

#endif