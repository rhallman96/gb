 #pragma once

#define READ false
#define WRITE true

#define BIT(n,b) ((n & (0x01 << b)) != 0x00)
#define SET(n,b) (n | (0x01 << b))
#define RES(n,b) (n & (~(0x01 << b)))

#include <list>
#include <stdint.h>

class DMATransferDevice;
class Rom;
class JoyPad;
class Audio;

/**
 * @author Rick Hallman
 * This class represents the Game Boy's memory bus.
 */
class Bus
{
public:

    /**
     * Constructor.
     * @param rom the game's ROM file
     */
    Bus( Rom* rom );
    ~Bus( void );

    /**
     * Write to the bus at a given address.
     * @param addr the address to access
     * @param val the data to write
     */
    void write( uint16_t addr, uint8_t val );

    /**
     * Access the bus at a given address.
     * @param addr the address to access
     * @param data the data read from / written to based on the write flag
     * @param write whether this is a read or write access
     */
    void access( uint16_t addr, uint8_t& data, bool write );

    /**
     * Access the bus at a given address WITHOUT looking at registered devices.
     * @param addr the address to access
     * @param data the data read from / written to based on the write flag
     * @param write whether this is a read or write access
     */
    void defaultAccess( uint16_t addr, uint8_t& data, bool write );

    /**
     * Accessor method for the joypad.
     */
    JoyPad* getJoyPad( void );
    
private:

    // ROM file
    Rom* mp_rom;

    // Joypad
    JoyPad* mp_joypad;

    // Default memory
    uint8_t* mp_memory;

    // DMA Transfer Device
    DMATransferDevice* mp_dmaReg;

};
