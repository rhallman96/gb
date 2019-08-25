#pragma once

#include "channel.h"

class Mixer;

class Wave : public Channel
{
public:

    /**
     * Constructor.
     * @param mixer the audio mixer containing this channel
     */
    Wave( Mixer* mixer );
    virtual ~Wave( void );

    void update( uint8_t ticks );
    Sint16 getAudio( void );

    void access( uint16_t addr, uint8_t& data, bool write );

private:

    virtual void trigger( void );
    
    // the audio mixer (used to access wave data)
    Mixer* mp_mixer;
    
};
