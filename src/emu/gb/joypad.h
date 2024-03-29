#pragma once

#include "devices.h"

#include <string>
using std::string;

/**
 * @author Rick Hallman
 *
 * This class represents the game's joypad as a
 * memory device.
 */
class JoyPad : public ReadOnlyDevice
{
public:

    static const uint16_t c_addr = 0xFF00;

    /**
     * Constructor.
     * @param baseDir the program's base directory
     */
    JoyPad( string baseDir="" );
    ~JoyPad( void );

    virtual void access( uint16_t addr, uint8_t& data, bool write );

    /**
     * Update and parse SDL keyboard input.
     */
    void readKeyboard( void );
    
    /**
     * Indicates whether or not a button changed from unpressed to
     * pressed.
     * @return true if yes, false otherwise
     */
    bool stateChanged( void );
    
protected:

    // config file
    static const string c_config;

    /**
     * Maps a button scancode to a key based on its key value.
     * @param key the string corresponding to a given key
     * @param value the button's scancode
     */
    void setKeyMapping( string key, int value );
    
    /**
     * Loads user-defined key bindings from keys.config.
     * @param baseDir the program's base directory
     */
    void loadConfigValues( string baseDir );
    
    /**
     * Sets the button values in the register's least significant
     * nibble.
     */
    void setButtonValues( void );
    
    // Key bindings
    uint8_t m_keyUp, m_keyDown, m_keyLeft, m_keyRight;
    uint8_t m_keyA, m_keyB, m_keyStart, m_keySelect;

    // Key states
    bool m_up, m_down, m_left, m_right;
    bool m_a, m_b, m_start, m_select;
    
    // State changed flag
    bool m_stateChanged;    
};
