#include "mbc3.h"

#include <iostream>
#include <fstream>

MBC3::MBC3( char* buffer, uint32_t bufferSize, bool ram, bool battery, bool timer, std::string savePath ) :
    Rom( buffer, bufferSize, ram, battery, savePath ),
    m_timer( timer ),
    m_romBankNum( 0x01 ),
    m_ramBankNum( 0x00 )
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
	    // read from RAM
	    uint16_t dest = addr - 0xA000;
	    dest += (m_ramBankNum * 0x2000);
	    data = mp_ramArray[ dest ];

	    // RTC register
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
	    if( data < 0x08 ) {
		m_ramBankNum = data;
	    }
	}
	else if( addr < 0x8000 )
	{
	    // latch clock data
	}
	else if( addr < 0xC000 )
	{
	    // write to RAM
	    uint16_t dest = addr - 0xA000;
	    dest += (m_ramBankNum * 0x2000);
	    mp_ramArray[ dest ] = data;
	    
	    // RTC register
	}
    }
}

void MBC3::save( void )
{
}

void MBC3::loadSave( void )
{
    using namespace std;
    
    if( !m_battery) { return; }

    ifstream file( m_savePath, ios::binary | ios::ate );
    if( file.is_open() )
    {
	unsigned int bufferSize = file.tellg();
        if( bufferSize != m_ramSize )
	{
	    cout << "Invalid save data." << endl;
	    return;
	}

	char* buffer;
	buffer = new char[bufferSize];
	file.seekg( 0, ios::beg );
	file.read( buffer, bufferSize );
	file.close();
	
	delete[] mp_ramArray;
	mp_ramArray = (uint8_t*)buffer;

	cout << "Save data loaded from " << m_savePath << endl;
    }
    else
    {
	cout << "No save file found." << endl;
    }
}

void MBC3::setRAMSize( void )
{
    if( m_ram )
    {    
	uint8_t ramType = mp_buffer[0x149];
        
	switch( ramType )
	{
            case 0x01:
	        m_ramSize = 0x800;
		break;
            case 0x02:
		m_ramSize = 0x2000;
	        break;
            case 0x03:
		m_ramSize = 0x8000;
		break;
            default:
		return;
	}

	mp_ramArray = new uint8_t[m_ramSize];
    }
}
