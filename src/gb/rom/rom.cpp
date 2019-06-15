#include "rom.h"
#include "mbc1.h"

#include <iostream>
#include <fstream>

Rom::Rom( char* buffer, uint32_t bufferSize, bool ram, bool battery, std::string savePath )
    : MemoryDevice( c_start, c_end ),
      m_savePath( savePath ),
      m_ram( ram ),
      m_battery( battery ),
      m_romMode( false ),
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
}

void Rom::access( uint16_t addr, uint8_t& data, bool write )
{
    if( write ) { return; }

    data = mp_buffer[addr];
}

void Rom::save( void )
{
}
