 #include "channel.h"

#include <iostream>

Channel::Channel( uint16_t start, uint16_t end ) :
    GenericDevice( start, end ),
    m_amplitude( 0 ),
    m_counter( 0 ),
    m_length( 0 ),
    m_lengthEnabled( false ),
    m_trigger( false ),
    m_enabled( false )
{
}

Channel::~Channel( void )
{
}

void Channel::access( uint16_t addr, uint8_t& data, bool write )
{
    this->GenericDevice::access( addr, data, write );

    if( ( write == WRITE ) &&
	( addr == ( m_start + 0x01 ) ) )
    {
        uint8_t val = data & 0x3F;
	m_length = 64 - val;
    }
    else if( ( write == WRITE ) &&
	     ( addr == m_end ) )
    {
	m_trigger = BIT( data, 7 );
	m_lengthEnabled = BIT( data, 6 );
    }
}
