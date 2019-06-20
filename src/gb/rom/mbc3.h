#pragma once

#include "rom.h"

/**
 * @author Rick Hallman
 * This class represents a ROM with an MBC3 bank controller.
 */
class MBC3 : public Rom
{
public:
    MBC3( char* buffer, uint32_t bufferSize, bool ram, bool battery, bool timer, std::string savePath );
    virtual ~MBC3( void );
    virtual void access( uint16_t addr, uint8_t& data, bool write );
    
private:

    // Whether or not this chip comes with a timer
    bool m_timer;

    // the ROM bank number
    uint8_t m_romBankNum;
    
    // RAM bank number / RTC register number
    uint8_t m_ramRtcBankNum;

    // RAM / RTC enabled flag
    bool m_ramRtcEnabled;

    // clock counter registers
    uint8_t m_clockCounterRegs[ 5 ];
};
