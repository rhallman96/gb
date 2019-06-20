#pragma once

#include "../bus.h"
#include "../devices.h"

#include <string>

/**
 * @author Rick Hallman
 *
 * This class represents the game cartridge as a
 * memory device.
 */
class Rom : public MemoryDevice
{
public:

    static const uint16_t c_start    = 0x0000;
    static const uint16_t c_end      = 0x7FFF;
    static const uint16_t c_ramStart = 0xA000;
    static const uint16_t c_ramEnd   = 0xBFFF;
    
    /**
     * Loads a new ROM from a file path.
     * @param string the path to the .gb file
     * @param savePath the save file's path
     * @return a newly constructed ROM, or NULL if failed
     */
    static Rom* Load( std::string path, std::string savePath );
    virtual ~Rom( void );
    virtual void access( uint16_t addr, uint8_t& data, bool write );

    /**
     * Write game data to the save file, if applicable.
     */
    virtual void save( void );
    
protected:

    /**
     * Constructor (called by Load).
     * @param buffer the file data
     * @param bufferSize the file buffer's size
     * @param ram whether or not this cartridge has RAM
     * @param battery whether or not this cartridge has a battery
     * @param savePath the save file's path
     */
    Rom( char* buffer, uint32_t bufferSize, bool ram, bool battery, std::string savePath );

    /**
     * Sets the RAM size based on the cartridge header.
     */
    void setRAMSize( void );
    
    // the ROM's save file
    std::string m_savePath;
    
    // whether or not this cartridge has ram or a battery
    bool m_ram, m_battery;

    // whether this is in ROM or RAM mode
    bool m_romMode;

    // external RAM
    uint8_t* mp_ramArray;
    int m_ramSize;
    
    // the cartridge data
    char* mp_buffer; uint32_t m_bufferSize;

    // MBC type
    typedef enum
    {
	C_ROM_ONLY             = 0x00,
	C_ROM_RAM              = 0x08,
	C_ROM_RAM_BATTERY      = 0x09,
	C_MBC1                 = 0x01,
	C_MBC1_RAM             = 0x02,
	C_MBC1_RAM_BATTERY     = 0x03,
	C_MBC3                 = 0x11,
	C_MBC3_RAM             = 0x12,
	C_MBC3_RAM_BATTERY     = 0x13,
	C_MBC3_TIM_BATTERY     = 0x0F,
	C_MBC3_TIM_RAM_BATTERY = 0x10
    } MBCType;
    
    static const uint16_t c_pageSize = 0x4000;
};
