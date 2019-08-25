#pragma once

#include "bus.h"
#include "rom/rom.h"
#include "z80.h"
#include "lcd.h"
#include "debug.h"
#include "devices.h"
#include "audio/mixer.h"

#include <string>
using std::string;

/**
 * @author Rick Hallman
 * This class represents a Game Boy at its most abstract level.
 */
class GB
{
public:
    /**
     * Constructor.
     * @param rom the loaded ROM file
     * @param baseDir the program's base directory
     */
    GB( Rom* rom, string baseDir="" );
    ~GB( void );

    /**
     * Executes the next instruction on the CPU and updates
     * hardware devices.
     * @return 0, or 1 if exiting
     */
    uint8_t update( void );
    
    /**
     * Called to close and save off external RAM.
     */
    void close( void );

    /**
     * Gets the number of ticks performed by this device.
     * @return the number of ticks taken so far
     */
    int getTicks( void );
   
    /**
     * Accessor method for the Game Boy's LCD.
     * @return a pointer to the LCD
     */
    LCD* getLCD( void );

    /**
     * Accessor method for the Game Boy's joypad.
     * @return a pointer to the JoyPad
     */
    JoyPad* getJoyPad( void );
    
private:

    /**
     * Updates hardware based on the number of ticks the previous instruction
     * took.
     * @param ticks the number of ticks the previous instruction took
     */
    void updateHardware( uint8_t ticks );
    
    /**
     * Updates timer and divider registers based on number of ticks.
     * @param ticks the number of ticks the previous instruction took
     */
    void updateTimers( uint8_t ticks );

    // Debugger
    Debug* mp_debug;

    // Hardware devices
    Rom* mp_rom;
    Bus* mp_bus;
    LCD* mp_lcd;
    Z80* mp_z80;
    Mixer* mp_mixer;
    
    // the cpu counter
    unsigned long m_counter;
    
};

