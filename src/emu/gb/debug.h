#pragma once

#ifndef DEBUG_MODE
#define DEBUG_MODE false
#endif

#include "z80.h"
#include "bus.h"

/**
 * @author Rick Hallman
 * Command line debugger.
 * Commands:
 *          r : run
 *          s : step
 *   b [addr] : set breakpoint to addr
 *          i : display registers
 *   i [addr] : display value at address
 */
class Debug
{
public:
    Debug( Z80* z80, Bus* bus );
    ~Debug( void );

    /**
     * Read input, evaluate, and execute.
     * @return 0, or 1 to quit
     */
    uint8_t repl( void );
    
private:
    
    Z80* mp_z80;
    Bus* mp_bus;


    // Step flag
    bool m_step;
    
    // The debugger's break address. 
    bool m_bFlag;
    uint16_t m_bAddr;

};
