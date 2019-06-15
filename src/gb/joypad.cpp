#include "joypad.h"
#include "bus.h"

#include <SDL2/SDL.h>

JoyPad::JoyPad( void ) : ReadOnlyDevice( c_addr, c_addr ),
			 m_up( false ),
			 m_down( false ),
			 m_left( false ),
			 m_right( false ),
			 m_a( false ),
			 m_b( false ),
			 m_start( false ),
			 m_select( false )
{
    // initialize all buttons to unpressed
    mp_memory[0] = 0xFF;

    // default key bindings
    m_keyUp = SDL_SCANCODE_UP;
    m_keyDown = SDL_SCANCODE_DOWN;
    m_keyLeft = SDL_SCANCODE_LEFT;
    m_keyRight = SDL_SCANCODE_RIGHT;
    m_keyA = SDL_SCANCODE_X;
    m_keyB = SDL_SCANCODE_Z;
    m_keyStart = SDL_SCANCODE_RETURN;
    m_keySelect = SDL_SCANCODE_RSHIFT;
}

JoyPad::~JoyPad( void )
{
}

void JoyPad::access( uint16_t addr, uint8_t& data, bool write )
{
    if( write )
    {
	uint8_t val = (data & 0xF0) | (mp_memory[0] & 0x0F);
        mp_memory[0] = val;
	this->setButtonValues();
    }
    else
    {
	data = mp_memory[0];
    }
}

void JoyPad::readKeyboard( void )
{
    const Uint8* keyboard;
    keyboard = SDL_GetKeyboardState(NULL);

    uint8_t reg = mp_memory[0];

    // direction keys
    bool nextUp = (keyboard[m_keyUp] != 0);
    bool nextDown = (keyboard[m_keyDown] != 0);
    bool nextLeft = (keyboard[m_keyLeft] != 0);
    bool nextRight = (keyboard[m_keyRight] != 0);

    // button keys
    bool nextA = (keyboard[m_keyA] != 0);
    bool nextB = (keyboard[m_keyB] != 0);
    bool nextStart = (keyboard[m_keyStart] != 0);
    bool nextSelect = (keyboard[m_keySelect] != 0);
    
    if( BIT( reg, 4 ) == false )
    {
	m_stateChanged = ( nextUp && !m_up ) ||
	    ( nextDown && !m_down ) ||
	    ( nextLeft && !m_left ) || 
	    ( nextRight && !m_right );

    }
    else if( BIT( reg, 5 ) == false)
    {
	m_stateChanged = ( nextA && !m_a ) ||
	    ( nextB && !m_b ) ||
	    ( nextStart && !m_start ) || 
	    ( nextSelect && !m_select );
    }

    m_up = nextUp;
    m_down = nextDown;
    m_left = nextLeft;
    m_right = nextRight;

    m_a = nextA;
    m_b = nextB;
    m_start = nextStart;
    m_select = nextSelect;

    this->setButtonValues();
}

bool JoyPad::stateChanged( void )
{
    if( m_stateChanged )
    {
	m_stateChanged = false;
	return true;
    }
    
    return false;
}
    
void JoyPad::setButtonValues( void )
{
    uint8_t reg = mp_memory[0];
    reg = reg | 0x0F;
    
    if( BIT( reg, 4 ) == false)
    {
	// direction keys
	if( m_up )
	{
	    reg = RES( reg, 2 );
	    reg = SET( reg, 3 );
	}
	else if( m_down )
	{
	    reg = SET( reg, 2 );
	    reg = RES( reg, 3 );
	}

	if( m_left )
	{
	    reg = SET( reg, 0 );
	    reg = RES( reg, 1 );
	}
	else if( m_right )
	{
	    reg = RES( reg, 0 );
	    reg = SET( reg, 1 );
	}
    }
    else if( BIT( reg, 5 ) == false)
    {
	// button keys
	if( m_a ) { reg = RES( reg, 0 ); }
	if( m_b ) { reg = RES( reg, 1 ); }
	if( m_start ) { reg = RES( reg, 3 ); }
	if( m_select ) { reg = RES( reg, 2 ); }
    }
    
    mp_memory[0] = reg;
}
