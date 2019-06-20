#include "mbc1.h"

#include <iostream>
#include <fstream>

MBC1::MBC1( char* buffer, uint32_t bufferSize, bool ram, bool battery, std::string savePath ) :
    Rom( buffer, bufferSize, ram, battery, savePath ),
    m_romBankNum( 0x01 ),
    m_ramBankNum( 0x00 ),
    m_ramEnabled( false )
{
    this->setRAMSize();
    this->loadSave();
}

MBC1::~MBC1( void )
{
}

void MBC1::access( uint16_t addr, uint8_t& data, bool write )
{
    if( write == READ )
    {
	if( addr < 0x4000 )
	{
	    // first ROM bank
	    data = mp_buffer[addr];
	}
	else if( addr < 0x8000 )
	{
	    uint32_t romAddr = addr - 0x4000;
	    romAddr += (m_romBankNum * 0x4000);
	    
	    data = mp_buffer[romAddr];
	}
	else if( m_ram && m_ramEnabled )
	{
	    // read from RAM
	    uint16_t dest = addr - 0xA000;
	    dest += (m_ramBankNum * 0x2000);
	    data = mp_ramArray[ dest ];
	}
    }
    else
    {
	if( addr < 0x2000 )
	{
	    // enable / disable external RAM
	    if( (data & 0x0F) == 0x0A ) {
		m_ramEnabled = true;
	    }
	    else {
		m_ramEnabled = false;
	    }
	}
	else if( addr < 0x4000 )
	{
	    // ROM bank number (bits 0-4)
	    uint16_t val = data & 0x1F;
	    if( val == 0x00 ) { val = 0x01; }
	    m_romBankNum = m_romBankNum & 0xE0;
	    m_romBankNum = m_romBankNum | val;
	}
	else if( addr < 0x6000)
	{
	    if( m_romMode )
	    {
		// upper bits of ROM bank number (bits 5-6)
		uint8_t val = data & 0x03;
		val = val << 5;
		m_romBankNum = m_romBankNum | val;
	    }
	    else
	    {
		// RAM bank number
		m_ramBankNum = data & 0x03;
	    }
	}
	else if( addr < 0x8000 )
	{
	    // ROM/RAM mode select
	    if( data == 0x00 ) { m_romMode = true; }
	    else { m_romMode = false; }
	}
	else if( m_ram && m_ramEnabled )
	{
	    // write to RAM
	    uint16_t dest = addr - 0xA000;
	    dest += (m_ramBankNum * 0x2000);
	    mp_ramArray[ dest ] = data;
	}
    }
}

