#pragma once

#include "channel.h"

/**
 * @author Rick Hallman
 * A square wave audio channel.
 */
class Square : public Channel
{
public:

    /**
     * Constructor.
     * @param addr the square wave's start address
     * @param hasSweep whether or not this square channel has sweeping (Default false)
     */
    Square( uint16_t addr, bool hasSweep = false );
    virtual ~Square( void );

    void update( uint8_t ticks );
    Sint16 getAudio( void );

    virtual void access( uint16_t addr, uint8_t& data, bool write );
    
private:

    virtual void trigger( void );

    /**
     * Updates the volume envelope.
     */
    void updateEnvelope( void );

    /**
     * Updates the frequency sweep.
     */
    void updateSweep( void );
    
    // frequency sweep
    bool m_hasSweep;
    int m_sweepPeriod;
    uint8_t m_shift;
    
    // the channel's volume
    uint8_t m_volume;
    uint8_t m_startVolume;
    int m_envelopePeriod;
    
    /**
     * Gets the wave's current frequency.
     * @return the wave's frequency
     */
    int getFrequency( void );

    /**
     * Gets the 11-bit frequency value.
     * @return the 11 bit frequency
     */
    int getFrequencyReg( void );
    
    /**
     * Sets the channel's 11-bit frequency.
     * @param freq the new frequency
     */
    void setFrequencyReg( int freq );

    /**
     * Gets the wave's current duty.
     * @param the wave's frequency
     * @return true if high, low otherwise
     */
    bool getDuty( int period );

    // the length decrement period
    static const int c_envelopePeriod = Z80::c_clockSpeed / 64;
    static const int c_sweepPeriod = Z80::c_clockSpeed / 128;

};


