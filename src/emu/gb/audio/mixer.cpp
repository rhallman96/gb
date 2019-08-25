#include "mixer.h"
#include "../z80.h"
#include "../bus.h"

#include <iostream>

const uint8_t Mixer::c_orValues[23] =
{ 0x80, 0x3F, 0x00, 0xFF, 0xBF,
  0xFF, 0x3F, 0x00, 0xFF, 0xBF,
  0x7F, 0xFF, 0x9F, 0xFF, 0xBF,
  0xFF, 0xFF, 0x00, 0x00, 0xBF,
  0x00, 0x00, 0x70 };

Mixer::Mixer( void ) :
    GenericDevice( c_start, c_end ),
    m_ticks( 0 ),
    m_buffer( new Sint16[ c_bufferSize ] ),
    m_bufferIndex( 0 )
{
    m_audioDevice = this->openDevice();
    mp_square1 = new Square( 0xFF10, true );
    mp_square2 = new Square( 0xFF15 );
    mp_wave = new Wave( this );
    mp_noise = new Noise();
    
    this->clearSoundRegisters();
    SDL_PauseAudioDevice( m_audioDevice, 0 );
}

Mixer::~Mixer( void )
{
    delete mp_square1; mp_square1 = NULL;
    delete mp_square2; mp_square2 = NULL;
    delete mp_wave; mp_wave = NULL;
    delete mp_noise;
    delete[] m_buffer; m_buffer = NULL;
    SDL_CloseAudioDevice( m_audioDevice );
}

void Mixer::access( uint16_t addr, uint8_t& data, bool write )
{
    if( addr >= 0xFF24 )
    {
	if( ( addr == 0xFF26 ) && ( write == WRITE ) )
	{
	    data &= 0x80;
	    if( BIT( data, 7 ) == false ) { this->clearSoundRegisters(); }    
	}
	
	this->GenericDevice::access( addr, data, write );	
    }
    else if( (addr >= mp_square1->getStart()) && (addr <= mp_square1->getEnd()) )
    {
	mp_square1->access( addr, data, write );
    }
    else if( (addr >= mp_square2->getStart()) && (addr <= mp_square2->getEnd()) )
    {
	mp_square2->access( addr, data, write );
    }

    if( write == READ ) { data |= c_orValues[ addr - 0xFF10 ]; }
}

void Mixer::update( uint8_t ticks )
{    
    long next = m_ticks + ticks;
    
    mp_square1->update( ticks );
    mp_square2->update( ticks );
    mp_wave->update( ticks );
    mp_noise->update( ticks );
    
    long ratio1 = ( m_ticks * c_sampleRate  ) / Z80::c_clockSpeed;
    long ratio2 = ( next * c_sampleRate ) / Z80::c_clockSpeed;
    
    if( ratio1 < ratio2 )
    {
	this->queueSamplesAndDelay();
    }

    m_ticks = next;
}

bool Mixer::isSoundPlaying( void )
{
    Uint32 size = SDL_GetQueuedAudioSize( m_audioDevice ); 
    return size >= (c_bufferSize * 2 * sizeof( Sint16 ));
}  

SDL_AudioDeviceID Mixer::openDevice( void )
{
    SDL_AudioSpec want, have;

    SDL_memset( &want, 0, sizeof( want ) );
    want.freq = c_sampleRate;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = c_bufferSize;
    
    SDL_AudioDeviceID device = SDL_OpenAudioDevice( NULL, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE );
    
    return device;
}

void Mixer::queueSamplesAndDelay( void )
{
    Sint16 s1 = mp_square1->getAudio();
    Sint16 s2 = mp_square2->getAudio();
    
    int lVolume = this->getLeftVolume();
    int rVolume = this->getRightVolume();
    
    Sint16 left = 0;
    Sint16 right = 0; 

    if( this->isSoundEnabled() )
    {
	if( this->isPlayingLeft( 1 ) ) { left += s1; }
	if( this->isPlayingRight( 1 ) ) { right += s1; }

	if( this->isPlayingLeft( 2 ) ) { left += s2; }
	if( this->isPlayingRight( 2 ) ) { right += s2; }

	left = left * lVolume / 8;
	right = right * rVolume / 8;
    }
    
    m_buffer[ m_bufferIndex ] = left;
    m_buffer[ m_bufferIndex + 1 ] = right;

    m_bufferIndex += 2;
    
    if( m_bufferIndex == c_bufferSize )
    {
	m_bufferIndex = 0;
        while( this->isSoundPlaying() ) { SDL_Delay( 1 ); }
	SDL_QueueAudio( m_audioDevice, m_buffer, c_bufferSize * sizeof( Sint16 ) );
    }
}

int Mixer::getLeftVolume( void )
{
    uint8_t nr50;
    this->access( 0xFF24, nr50, READ );

    int volume = (nr50 >> 4) & 0x07;
    return volume;
}

int Mixer::getRightVolume( void )
{
    uint8_t nr50;
    this->access( 0xFF24, nr50, READ );

    int volume = nr50 & 0x07;
    return volume;
}

bool Mixer::isPlayingLeft( int channel )
{     
    uint8_t nr51;
    this->access( 0xFF25, nr51, READ );
    
    return BIT( nr51, (channel - 1) );
}

bool Mixer::isPlayingRight( int channel )
{    
    uint8_t nr51;
    this->access( 0xFF25, nr51, READ );
    
    return BIT( nr51, (3 + channel) );
}

bool Mixer::isSoundEnabled( void )
{
    uint8_t nr52;
    this->access( 0xFF26, nr52, READ );
    bool enabled = BIT( nr52, 7 );
    
    return enabled;	
}

void Mixer::clearSoundRegisters( void )
{
    uint8_t zero = 0;
    
    for( uint16_t addr = c_start; addr < 0xFF26; addr++ ) {
	this->access( addr, zero, WRITE );
    }
}
