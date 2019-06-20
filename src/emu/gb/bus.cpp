#include "bus.h"
#include "debug.h"
#include "devices.h"
#include "joypad.h"
#include "rom/rom.h"

#include <iostream>
#include <list>
#include <iterator>

using namespace std;


Bus::Bus( Rom* rom ) :
    mp_rom( rom ),
    mp_joypad( new JoyPad() )
{
    mp_dmaReg = new DMATransferDevice( this );
    mp_memory = new uint8_t[65536];
}

Bus::~Bus( void )
{
    delete mp_joypad; mp_joypad = NULL;
    delete mp_dmaReg; mp_dmaReg = NULL;
    delete[] mp_memory; mp_memory = NULL;
}

void Bus::write( uint16_t addr, uint8_t val )
{
    uint8_t reg = val;
    this->access( addr, reg, WRITE );
}

void Bus::access( uint16_t addr, uint8_t& data, bool write )
{
    if( addr <= Rom::c_end ||
	((addr >= Rom::c_ramStart) && (addr <= Rom::c_ramEnd)))
    {
	// ROM access
	mp_rom->access( addr, data, write );
    }
    else if( addr == JoyPad::c_addr )
    {
	// JoyPad access
	mp_joypad->access( addr, data, write );
    }
    else if( addr == DMATransferDevice::c_addr )
    {
	// DMA transfer
	mp_dmaReg->access( addr, data, write );
    }
    else if( ( (addr == 0xFF44) || (addr == 0xFF04) )
	     && write )
    {
	// LCDC Y and divider
	mp_memory[addr] = 0;
    }
    else
    {
	this->defaultAccess( addr, data, write );
    }    
}

void Bus::defaultAccess( uint16_t addr, uint8_t& data, bool write )
{
    if( write == READ ) { data = mp_memory[addr]; }
    else { mp_memory[addr] = data; }
}

JoyPad* Bus::getJoyPad( void )
{
    return mp_joypad;
}
