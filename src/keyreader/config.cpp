#include "config.h"

#include <fstream>

Config::Config( string path ) :
    m_path( path )
{
}

Config::~Config( void )
{
}

void Config::addMapping( string button, int value )
{
    m_buttonMap[ button ] = value;
}

bool Config::write( void )
{
    using namespace std;

    ofstream file( m_path );

    if( file.is_open() == false ) { return false; }

    for( pair<string,int> item : m_buttonMap )
    {
	file << item.first << " " << item.second << endl;
    }
    
    return true;
}

