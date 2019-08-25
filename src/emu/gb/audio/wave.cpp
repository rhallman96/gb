#include "wave.h"

Wave::Wave( Mixer* mixer ) :
    Channel( 0xFF1A, 0xFF1E ),
    mp_mixer( mixer )
{
}

Wave::~Wave( void )
{
}

void Wave::update( uint8_t ticks )
{
}

Sint16 Wave::getAudio( void )
{
    return 0;
}

void Wave::access( uint16_t addr, uint8_t& data, bool write )
{
}

void Wave::trigger( void )
{
}
