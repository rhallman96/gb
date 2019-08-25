#include "gb.h"
#include "joypad.h"

#include <iostream>

GB::GB( Rom* rom, string baseDir )
    : mp_rom( rom ),
      m_counter( 0x0000 )
{
    mp_mixer = new Mixer();
    mp_bus = new Bus( rom, mp_mixer, baseDir );    
    mp_z80 = new Z80( mp_bus );
    mp_lcd = new LCD( mp_z80, mp_bus );
    
    // initialize debugger
    mp_debug = new Debug( mp_z80, mp_bus );
}

GB::~GB( void )
{
    delete mp_z80; mp_z80 = NULL;
    delete mp_bus; mp_bus = NULL;    
    delete mp_lcd; mp_lcd = NULL;
    delete mp_mixer; mp_mixer = NULL;
    delete mp_debug; mp_debug = NULL;
}

uint8_t GB::update( void )
{    
    uint8_t ticks = mp_z80->executeNextInstruction();
    updateHardware( ticks );
    
    return mp_debug->repl();
}

void GB::close( void )
{
    mp_rom->save();
}

int GB::getTicks( void )
{
    return m_counter;
}

LCD* GB::getLCD( void )
{
    return mp_lcd;
}

JoyPad* GB::getJoyPad( void )
{
    return mp_bus->getJoyPad();
}

void GB::updateHardware( uint8_t ticks )
{
    this->updateTimers( ticks );
    mp_lcd->update( ticks );
    mp_mixer->update( ticks );
    if( mp_bus->getJoyPad()->stateChanged() )
    {
	mp_z80->triggerInterrupt( Z80::c_joypad );
    }
    
    m_counter += ticks;
}

void GB::updateTimers( uint8_t ticks )
{
    unsigned long prev = m_counter;
    unsigned long curr = m_counter + ticks;
    
    if( (prev / 256) < (curr / 256) )
    {
	// DIV (0xFF04) register increments every 256 cycles (16384Hz)
	uint8_t divider;
	mp_bus->defaultAccess( 0xFF04, divider, READ );
	mp_bus->defaultAccess( 0xFF04, ++divider, WRITE );
    }

    // increment TIMA (0xFF05) based on TAC (0xFF07)
    uint8_t tima, tma, tac;
    mp_bus->defaultAccess( 0xFF05, tima, READ );
    mp_bus->defaultAccess( 0xFF06, tma, READ );
    mp_bus->defaultAccess( 0xFF07, tac, READ );
    
    if( BIT( tac, 2 ) ) // if running flag set, increment TIMA
    {
	uint16_t t;
	// game boy runs at 4194304 Hz	
	switch( tac & 0x03 )
	{
	case 0x00: // 4096 hz
	    t = 1024; break;
	case 0x01: // 262144 hz
	    t = 16; break;
	case 0x02: // 65536 hz
	    t = 64; break;
	case 0x03: // 16384 hz
	default:
	    t = 256;
	    break;
	}

	if( (prev / t) < (curr / t) )
	{
	    tima += (uint8_t)( (curr / (long)t) - (prev / (long)t) );
	
	    // trigger timer interrupt if overflow
	    if( tima == 0x00 )
	    {
		tima = tma;
		mp_z80->triggerInterrupt( Z80::c_timer );
	    }

	    mp_bus->defaultAccess( 0xFF05, tima, WRITE );
	}	
    }
}

