#include "square.h"
#include "../z80.h"

#include <iostream>

Square::Square( uint16_t addr, bool hasSweep ) :
    Channel( addr, addr + 0x04 ),
    m_hasSweep( hasSweep ),
    m_volume( 0 ),
    m_startVolume( 0 ),
    m_envelopePeriod( 0 ),
    m_sweepPeriod( 0 ),
    m_shift( 0 )
{
}

Square::~Square( void )
{
}

void Square::update( uint8_t ticks )
{
    if( m_trigger ) { this->trigger(); }
    
    unsigned long next = m_counter + ticks;
    
    if( m_lengthEnabled &&
	( ( m_counter / c_lengthPeriod ) < ( next / c_lengthPeriod ) ) &&
	( m_length > 0 ) )
    {
	m_length--;
	if( m_length == 0 ) { m_enabled = false; }
    }

    if( m_envelopePeriod != 0 )
    {
	if( ( m_counter / m_envelopePeriod ) < ( next / m_envelopePeriod ) )
	{
	    this->updateEnvelope();
	}
    }

    if( m_hasSweep && ( m_sweepPeriod != 0 ) )
    {
	if( ( m_counter / m_sweepPeriod ) < ( next / m_sweepPeriod ) )
	{
	    this->updateSweep();
	}
    }
    
    m_counter = next;
}

Sint16 Square::getAudio( void )
{
    if( m_enabled == false ) { return 0; }
    
    Sint16 amplitude = c_maxVol;
    int frequency = this->getFrequency();
    int period = Z80::c_clockSpeed / frequency;

    if( this->getDuty( period ) == false ) { amplitude *= -1; }

    amplitude *= m_volume;
    amplitude /= 16;
    
    return amplitude;
}

void Square::access( uint16_t addr, uint8_t& data, bool write )
{
    this->Channel::access( addr, data, write );

    if( ( addr == m_start ) &&
	     ( write == WRITE ) )
    {
	uint8_t time = ( data >> 4 ) & 0x07;
	m_sweepPeriod = (int)time * c_sweepPeriod;
	
	m_shift = (data & 0x07);
    }
    else if( (addr == ( m_start + 0x02 ) ) &&
	( write == WRITE ) )
    {
	m_envelopePeriod = c_envelopePeriod * ( data & 0x07 );
	m_startVolume = ( data >> 4 );
    }    
}

void Square::trigger( void )
{
    m_trigger = false;
    m_enabled = true;

    if( m_length == 0 ) { m_length = 64; }

    m_counter = 0;
    m_volume = m_startVolume;
}

void Square::updateEnvelope( void )
{
    uint8_t envelope;
    this->access( m_start + 2, envelope, READ );

    bool direction = BIT( envelope, 3 );

    if( direction && ( m_volume < 15 ) )
    {
	m_volume++;
    }
    else if( m_volume > 0 )
    {
	m_volume--;
    }
}

void Square::updateSweep( void )
{
    uint8_t sweep;
    this->access( m_start, sweep, READ );

    bool direction = BIT( sweep, 3 );
    int freq = this->getFrequencyReg();
    
    int next = freq;
    next /= (1 << m_shift);
    
    if( direction )
    {
	next = freq - next;
	if( next < 0 ) { next = 0; m_enabled = false; }
    }
    else
    {
	next = freq + next;
	if( next >= 2048 ) { next = 2047; m_enabled = false; }
    }

    if( ( next >= 2048 ) || ( next < 0 ) )
    {
	next = 0;
	m_enabled = false;
    }
    
    this->setFrequencyReg( next );
}

int Square::getFrequency( void )
{
    int freq = this->getFrequencyReg();
    freq = 131072 / ( 2048 - freq );
    return freq;
}

int Square::getFrequencyReg( void )
{
    int freq = 0;
    
    uint8_t high, low;

    this->access( m_start + 3, low, READ );
    this->access( m_start + 4, high, READ );

    high &= 0x07;

    freq += high;
    freq = freq << 8;
    freq += low;

    return freq;
}

void Square::setFrequencyReg( int freq )
{
    uint8_t highf = ( freq >> 8 );
    highf &= 0x07;
    uint8_t lowf = ( freq & 0xFF );

    uint8_t high;
    this->access( m_start + 4, high, READ );
    high &= 0xF8;
    high |= highf;
    
    this->access( m_start + 3, lowf, WRITE );
    this->access( m_start + 4, highf, WRITE );
}

bool Square::getDuty( int period )
{
    uint8_t bits, duty;
    this->access( m_start + 1, bits, READ );
    bits = bits >> 6;
    switch( bits )
    {
        case 0x00:
	    duty = 0x01;
	    break;
        case 0x01:
	    duty = 0x81;
	    break;
        case 0x02:
	    duty = 0x87;
	    break;
        case 0x03:
        default:
	    duty = 0x7E;
	    break;
    }

    uint8_t b = (m_counter / (period / 8) ) % 8;
    return BIT( duty, b );
}
