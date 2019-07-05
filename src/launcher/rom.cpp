#include "rom.h"

#include <unistd.h>

// file extensions
const string Rom::c_romExtension = ".gb";
const string Rom::c_saveExtension = ".sav";

Rom::Rom( string name, string romPath, string savePath ) :
    m_name( name ),
    m_romPath( romPath ),
    m_savePath( savePath )
{
}

string Rom::getRomPath( void )
{
    return m_romPath;
}

string Rom::getSavePath( void )
{
    return m_savePath;
}

