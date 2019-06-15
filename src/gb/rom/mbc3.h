#pragma once

#include "rom.h"

/**
 * @author Rick Hallman
 * This class represents a ROM with an MBC3 bank controller.
 */
class MBC3 : public Rom
{
public:
    MBC3( char* buffer, uint32_t bufferSize, bool ram, bool battery, std::string savePath );
    virtual ~MBC3( void );
    virtual void access( uint16_t addr, uint8_t& data, bool write );
    virtual void save( void );
    
private:
};
