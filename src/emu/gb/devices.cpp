#include "devices.h"
#include "bus.h"

MemoryDevice::MemoryDevice( uint16_t start, uint16_t end ) :
    m_start( start ),
    m_end( end )
{
}

MemoryDevice::~MemoryDevice( void )
{
}

uint16_t MemoryDevice::getStart( void )
{
    return m_start;
}

uint16_t MemoryDevice::getEnd( void )
{
    return m_end;
}

GenericDevice::GenericDevice( uint16_t start, uint16_t end )
    : MemoryDevice( start, end ),
      mp_memory( NULL )
{
    uint16_t size = end - start + 1;
    mp_memory = new uint8_t[size];
}

GenericDevice::~GenericDevice( void )
{
    if( mp_memory != NULL )
    {
	delete[] mp_memory;
	mp_memory = NULL;
    }
}

void GenericDevice::access( uint16_t addr, uint8_t& data, bool write )
{
    uint16_t index = addr - m_start;
    
    if( write )
    {
	mp_memory[index] = data;
    }
    else
    {
	data = mp_memory[index];
    }
}

ReadOnlyDevice::ReadOnlyDevice( uint16_t start, uint16_t end )
    : GenericDevice( start, end )
{
}

ReadOnlyDevice::~ReadOnlyDevice( void )
{
}

void ReadOnlyDevice::access( uint16_t addr, uint8_t& data, bool write )
{
    uint16_t index = addr - m_start;
    
    if( write )
    {	
	mp_memory[index] = 0;
    }
    else
    {
	data = mp_memory[index];
    }
}

void ReadOnlyDevice::forceWrite( uint16_t addr, uint8_t data )
{
    uint16_t index = addr - m_start;
    mp_memory[index] = data;
}

DMATransferDevice::DMATransferDevice( Bus* bus )
    : MemoryDevice( c_addr, c_addr ),
      mp_bus( bus )
{
}

DMATransferDevice::~DMATransferDevice( void )
{
}

void DMATransferDevice::access( uint16_t addr, uint8_t& data, bool write )
{
    if( write == READ ) { return; }

    uint16_t src = (uint16_t)data << 8;

    for( uint16_t i = 0x00; i <= 0x9F; i++ )
    {
	uint8_t val;
	mp_bus->access( src + i, val, READ );
	mp_bus->access( 0xFE00 + i, val, WRITE );
    }    
}
