#pragma once

#include "rom.h"

/**
 * @author Rick Hallman
 * This class represents a ROM with an MBC1 bank controller.
 */
class MBC1 : public Rom
{
public:

    /**
     * Constructor.
     * @param buffer the cartridge buffer
     * @param bufferSize the size of this buffer
     * @param ram whether or not this cartridge has built in RAM
     * #param battery whether or not this cartridge ha a built-in battery
     * @param savePath the game file's save path
     */
    MBC1( char* buffer, uint32_t bufferSize, bool ram, bool battery, std::string savePath );
    virtual ~MBC1( void );
    virtual void access( uint16_t addr, uint8_t& data, bool write );
    
private:

    // the rom and ram bank numbers
    uint8_t m_romBankNum;
    uint8_t m_ramBankNum;

    // RAM enabled flag
    bool m_ramEnabled;
};
