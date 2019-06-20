#include "mbc3.h"

#include <iostream>
#include <fstream>

MBC3::MBC3( char* buffer, uint32_t bufferSize, bool ram, bool battery, bool timer, std::string savePath ) :
    Rom( buffer, bufferSize, ram, battery, savePath ),
    m_timer( timer ),
    m_romBankNum( 0x01 ),
    m_ramRtcBankNum( 0x00 )
{
    this->setRAMSize();
    this->loadSave();
}

MBC3::~MBC3( void )
{
    if( mp_ramArray != NULL )
    {
	delete[] mp_ramArray;
	mp_ramArray = NULL;
    }
}

void MBC3::access( uint16_t addr, uint8_t& data, bool write )
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
	else if( addr < 0xC000 )
	{
	    if( m_ramRtcBankNum < 0x04 ) {	    
		// read from RAM
		uint16_t dest = addr - 0xA000;
		dest += (m_ramRtcBankNum * 0x2000);
		data = mp_ramArray[ dest ];
	    }
	    else if( m_timer )
	    {
		// RTC register
		uint8_t num = m_ramRtcBankNum - 0x08;
		data = m_clockCounterRegs[ num ];
	    }
	}
    }
    else
    {
	if( addr < 0x2000 )
	{
	    // RAM and timer enable
	    if( (data & 0xF) == 0xA ) {
		m_ramRtcEnabled = true;
	    }
	    else {
		m_ramRtcEnabled = false;
	    }
	}
	else if( addr < 0x4000 )
	{
	    // ROM bank number
	    uint8_t bankNum = data & 0x7F;
	    if( bankNum == 0x0 ) { bankNum = 0x1; }
	    m_romBankNum = bankNum;
	}
	else if( addr < 0x6000 )
	{
	    // RAM bank num or RTC register select
	    m_ramRtcBankNum = data;
	}
	else if( addr < 0x8000 )
	{
	    // latch clock data
	}
	else if( addr < 0xC000 )
	{
	    if( m_ramRtcBankNum < 0x04 ) {	    
		// write to RAM
		uint16_t dest = addr - 0xA000;
		dest += (m_ramRtcBankNum * 0x2000);
		mp_ramArray[ dest ] = data;
	    }
	    else if( m_timer )
	    {
		// RTC register
		uint8_t num = m_ramRtcBankNum - 0x08;
		m_clockCounterRegs[ num ] = data;
	    }		
	}
    }
}

