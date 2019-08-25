#include "rom.h"
#include "mbc1.h"
#include "mbc3.h"

#include <iostream>
#include <fstream>

Rom::Rom( char* buffer, uint32_t bufferSize, bool ram, bool battery, std::string savePath )
    : MemoryDevice( c_start, c_end ),
      m_savePath( savePath ),
      m_ram( ram ),
      m_battery( battery ),
      m_romMode( false ),
      mp_ramArray( NULL ),
      mp_buffer( buffer ),
      m_bufferSize( bufferSize )
{
}

Rom* Rom::Load( std::string path, std::string savePath )
{
    using namespace std;
    
    // load the file
    ifstream file( path, ios::binary | ios::ate );
    if( file.good() == false ) { return NULL; }

    // load the ROM buffer
    unsigned int bufferSize = file.tellg();
    char* buffer;
    buffer = new char[bufferSize];
    file.seekg( 0, ios::beg );
    file.read( buffer, bufferSize );
    file.close();
    
    // MBC type is stored at 0x0147 in cartridge header
    uint8_t type = buffer[0x147];

    Rom* rom = NULL;

    switch( type )
    {
    case C_ROM_ONLY:
	rom = new Rom( buffer, bufferSize, false, false, savePath );
	break;
    case C_ROM_RAM:
	rom = new Rom( buffer, bufferSize, true, false, savePath );
	break;
    case C_ROM_RAM_BATTERY:
	rom = new Rom( buffer, bufferSize, true, true, savePath );
	break;
    case C_MBC1:
	rom = new MBC1( buffer, bufferSize, false, false, savePath );
	break;
    case C_MBC1_RAM:
	rom = new MBC1( buffer, bufferSize, true, false, savePath );
	break;
    case C_MBC1_RAM_BATTERY:
	rom = new MBC1( buffer, bufferSize, true, true, savePath );
	break;
    case C_MBC3:
	rom = new MBC3( buffer, bufferSize, false, false, false, savePath );
	break;
    case C_MBC3_RAM:
	rom = new MBC3( buffer, bufferSize, true, false, false, savePath );
	break;
    case C_MBC3_RAM_BATTERY:
	rom = new MBC3( buffer, bufferSize, true, true, false, savePath );
	break;
    case C_MBC3_TIM_BATTERY:
	rom = new MBC3( buffer, bufferSize, false, true, true, savePath );
	break;
    case C_MBC3_TIM_RAM_BATTERY:
	rom = new MBC3( buffer, bufferSize, true, true, true, savePath );
	break;
    default:
	break;
    }
    
    return rom;
}

Rom::~Rom( void )
{
    if( mp_buffer != NULL )
    {
	delete[] mp_buffer;
	mp_buffer = NULL;
    }

    if( mp_ramArray != NULL )
    {
	delete[] mp_ramArray;
	mp_ramArray = NULL;
    }

}

void Rom::access( uint16_t addr, uint8_t& data, bool write )
{
    if( write ) { return; }

    data = mp_buffer[addr];
}

void Rom::setRAMSize( void )
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
	    case 0x04:
	        m_ramSize = 0x20000;
	        break;
            case 0x05:
		m_ramSize = 0x10000;
		break;
            default:
		m_ramSize = 0;
		return;
	}

	mp_ramArray = new uint8_t[m_ramSize];
    }
}

void Rom::save( void )
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

void Rom::loadSave( void )
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
