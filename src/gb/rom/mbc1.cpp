#include "mbc1.h"

#include <iostream>
#include <fstream>

MBC1::MBC1( char* buffer, uint32_t bufferSize, bool ram, bool battery, std::string savePath ) :
    Rom( buffer, bufferSize, ram, battery, savePath ),
    mp_ramArray( NULL ),
    m_ramSize( 0 ),
    m_ramBankNum( 0x00 ),
    m_romBankNum( 0x01 )
{
    this->setRAMSize();
    this->loadSave();
}

MBC1::~MBC1( void )
{
    if( mp_ramArray != NULL )
    {
	delete[] mp_ramArray;
	mp_ramArray = NULL;
    }
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
	else if( m_ram )
	{
	    // read from RAM
	    

	    data = mp_ramArray[ addr - 0xA000 ];
	}
    }
    else
    {
	if( (addr >= 0x2000) && (addr <= 0x3FFF) )
	{
	    // ROM bank number (bits 0-4)
	    uint16_t val = data & 0x1F;
	    if( val == 0x00 ) { val = 0x01; }
	    m_romBankNum = m_romBankNum & 0xE0;
	    m_romBankNum = m_romBankNum | val;
	}
	else if( (addr >= 0x4000) && (addr <= 0x5FFF) )
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
	else if( addr <= 0x7FFF )
	{
	    // ROM/RAM mode select
	    if( data == 0x00 ) { m_romMode = true; }
	    else { m_romMode = false; }
	}
	else if( m_ram && (addr >= 0xA000) )
	{
	    // write to RAM
	    mp_ramArray[ addr - 0xA000 ] = data;
	}
    }
}

void MBC1::save( void )
{
    using namespace std;
    
    if( !m_battery ) { return; }

    ofstream file( m_savePath, ios::binary );
    if( file.is_open() )
    {
	file.write( (char*)mp_ramArray, m_ramSize );
	file.close();
	cout << "Wrote save data to " << m_savePath << endl;
    }
    else
    {
	cout << "Unable to save battery-backed RAM." << endl;
    }
}

void MBC1::loadSave( void )
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

void MBC1::setRAMSize( void )
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
