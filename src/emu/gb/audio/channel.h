#pragma once

#include "../devices.h"
#include "../z80.h"

#include <SDL2/SDL_audio.h>

/**
 * @author Rick Hallman
 * Generic audio channel class.
 */
class Channel : public GenericDevice
{
public:

    /**
     * Constructor.
     * @param start the starting memory address
     * @param end the ending memory address
     */
    Channel( uint16_t start, uint16_t end );
    virtual ~Channel( void );

    virtual void access( uint16_t addr, uint8_t& data, bool write );
    
    /**
     * Updates the audio channel.
     * @param ticks the number of ticks the previous instruction took
     */
    virtual void update( uint8_t ticks ) = 0;
    
    /**
     * Get the current audio output.
     * @return the audio as a signed 16 bit integer
     */
    virtual Sint16 getAudio( void ) = 0;

protected:

    // trigger event
    virtual void trigger() = 0;
    
    // the wave's amplitude
    Sint16 m_amplitude;    

    // the number of ticks that have passed
    unsigned long m_counter;

    // length counter
    int m_length;
    bool m_lengthEnabled;

    // trigger event
    bool m_trigger;

    // enabled flag
    bool m_enabled;
    
    // the maximum channel volume
    static const Sint16 c_maxVol = 4000;

    // the length decrement period
    static const Sint16 c_lengthPeriod = Z80::c_clockSpeed / 256;
};
