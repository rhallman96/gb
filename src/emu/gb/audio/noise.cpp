#include "noise.h"

#include <stdlib.h>
#include <time.h>

Noise::Noise( void ) :
    Channel( c_start, c_end )
{
    // random number generator seed
    srand( time( NULL ) );
}

Noise::~Noise( void )
{
}

void Noise::update( uint8_t ticks )
{
    if( m_trigger ) { this->trigger(); }
    unsigned long next = m_counter + ticks;


    
    m_counter = next;
}

Sint16 Noise::getAudio( void )
{    
    return 0;
}

void Noise::trigger( void )
{
}

void Noise::access( uint16_t addr, uint8_t& data, bool write )
{
    this->GenericDevice::access( addr, data, write );

    if( write )
    {
	switch( addr )
	{
            case 0xFF20:
	        this->setLength( data );
		break;
	    default:
	        break;
	}
    }
}

void Noise::setLength( uint8_t data )
{
    int val = data & 0x3F;
    m_length = ( 64 - val ) * c_lengthPeriod;
}
