#include "debug.h"

#include <iostream>
#include <regex>
#include <string>
#include <sstream>


Debug::Debug( Z80* z80, Bus* bus )
    : mp_z80( z80 ),
      mp_bus( bus ),
      m_step( true ),
      m_bFlag( false ),
      m_bAddr( 0x0000 )
{
}

uint8_t Debug::repl( void )
{
    using namespace std;

    if( DEBUG_MODE == false ) { return 0; }

    uint16_t addr = mp_z80->getPC();

    // Only debug if stepping or at breakpoint
    if( (m_step == false) &&
	(!((addr == m_bAddr) && m_bFlag)) ) { return 0; }
    
    m_step = false;
    string input;
    while( true )
    {
	cout << "gb>";
	getline( cin, input );
	
	if( input.compare("q") == 0 )
	{
	    return 1;
	}
	else if( regex_match( input, regex("i ([0-9]|[a-f]|[A-F]){1,4}") ) )
	{
	    string second = input.substr( 1, ' ' );
	    stringstream ss;
	    ss << second;

	    uint16_t addr;
	    ss >> hex >> addr;

	    uint8_t val;
	    mp_bus->access( addr, val, READ );

	    cout << "Value at " << hex << addr
		 << ": " << hex << (int)val << endl;
	}
	else if( regex_match( input, regex("b ([0-9]|[a-f]|[A-F]){1,4}") ) )
	{   
	    string second = input.substr( 1, ' ' );
	    stringstream ss;
	    ss << second;

	    uint16_t addr;
	    ss >> hex >> addr;

	    m_bAddr = addr;
	    m_bFlag = true;

	    cout << endl << "Breakpoint set at " << hex << (int)m_bAddr
		 << endl;
	}	
	else if( input.compare("s") == 0 )
	{
	    m_step = true;
	    break;
	}
	else if( input.compare("i") == 0 )
	{
	    mp_z80->printStatus();
	}
	else if( input.compare("r") == 0 )
	{
	    break;
	}
    }

    return 0;
}

Debug::~Debug( void )
{
}
