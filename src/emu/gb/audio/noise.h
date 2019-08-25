#pragma once

#include "channel.h"

/**
 * @author Rick Hallman
 * A noise audio channel.
 */
class Noise : public Channel
{
public:

    static const uint16_t c_start = 0xFF20;
    static const uint16_t c_end = 0xFF23;
    
    Noise( void );
    virtual ~Noise( void );

    void update( uint8_t ticks );
    Sint16 getAudio( void );

    void access( uint16_t addr, uint8_t& data, bool write );
    
private:

    virtual void trigger( void );
    
    /**
     * Sets the sound length.
     * @param data the data being written to this device
     */
    void setLength( uint8_t data );
    
    // sound length
    static const int c_lengthPeriod = 16384; // 4194304 hz / 256
    int m_length;

    // volume envelope
    uint8_t m_volume, m_startVolume;
    
};
