#pragma once

#include <stdint.h>

class Bus;

/**
 * @author Rick Hallman
 * This class represents a memory-mapped device in the bus.
 */
class MemoryDevice
{
public:

    /**
     * Constructor.
     * @param start the device's starting memory address
     * @param end the device's ending memory address
     */
    MemoryDevice( uint16_t start, uint16_t end );
    ~MemoryDevice( void );

    /**
     * Access the device at a given address.
     * @param addr the address to access
     * @param data the data read from / written to based on the write flag
     * @param write whether this is a read or write access
     */
    virtual void access( uint16_t addr, uint8_t& data, bool write ) = 0;
    
    /**
     * Accessor method for the device's start address.
     * @return the device's start address
     */
    uint16_t getStart( void );

    /**
     * Accessor method for the device's end address.
     * @return the device's end address
     */
    uint16_t getEnd( void );
    
protected:
    
    uint16_t m_start, m_end;
};

/**
 * @author Rick Hallman
 * This class represents a generic memory-mapped device that can
 * be read from and written to.
 */
class GenericDevice : public MemoryDevice
{
public:

    /**
     * Constructor.
     * @param start the device's starting memory address
     * @param end the device's ending memory address
     */
    GenericDevice( uint16_t start, uint16_t end );
    virtual ~GenericDevice( void );

    virtual void access( uint16_t addr, uint8_t& data, bool write );
    
protected:

    uint8_t* mp_memory;
};

/**
 * @author Rick Hallman
 * A read-only memory device. When written to, resets the value to 0.
 */
class ReadOnlyDevice : public GenericDevice
{
public:
    /**
     * Constructor.
     * @param start the device's starting memory address
     * @param end the device's ending memory address
     */
    ReadOnlyDevice( uint16_t start, uint16_t end );
    virtual ~ReadOnlyDevice( void );

    virtual void access( uint16_t addr, uint8_t& data, bool write );

    /**
     * Writes to this readonly device without resetting its value.
     * @param addr the access memory address
     * @param data the data to wriet
     */
    void forceWrite( uint16_t addr, uint8_t data );
};

/**
 * When this device is written to, a DMA transfer is executed.
 * Hard coded to 0xFF46.
 */
class DMATransferDevice : public MemoryDevice
{
public:

    static const uint16_t c_addr = 0xFF46;

    /**
     * Constructor.
     * @param bus the device's bus
     */
    DMATransferDevice( Bus* bus );
    virtual ~DMATransferDevice( void );

    virtual void access( uint16_t addr, uint8_t& data, bool write );

private:
    Bus* mp_bus;
};

