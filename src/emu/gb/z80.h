#pragma once

#include "bus.h"
#include <stdint.h>

/**
 * @author Rick Hallman
 *
 * This class emulates the Game Boy's CPU.
 * This is a modified version of the Zilog Z80.
 *
 * Opcode reference: http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
 */
class Z80
{
public:

    // the processor's clock speed
    static const int c_clockSpeed = 4194304;
    
    /**
     * Constructor.
     * @param bus a reference to the device's bus
     */
    Z80( Bus* bus );
    ~Z80( void );

    /**
     * Gets the value of the program counter.
     */
    uint16_t getPC( void );
    
    /**
     * Prints the CPU's current status to the command line.
     */
    void printStatus( void );
    
    /**
     * Executes the next instruction pointed to by the PC.
     * @return the number of ticks this instruction took
     */
    uint8_t executeNextInstruction( void );
    
    // Interrupt masks
    static const uint8_t c_vBlank = 0x01;
    static const uint8_t c_LCDStat = 0x02;
    static const uint8_t c_timer = 0x04;
    static const uint8_t c_serial = 0x08;
    static const uint8_t c_joypad = 0x10;

    /**
     * Trigger a CPU interrupt.
     * @param mask the mask to apply
     */
    void triggerInterrupt( uint8_t mask );
    
private:

    /**
     * Halts the CPU.
     * @return the number of interrupts this instruction took.
     */
    uint8_t halt( void );
    
    /**
     * Checks for an interrupt. Executed immediately after each
     * instruction is called.
     */
    void checkInterrupts( void );
    
    /**
     * Sets a given flag.
     * @param mask the flag's mask
     * @param value the flag's value (default true)
     */
    void setFlag( uint8_t mask, bool value=true );

    /**
     * Gets the value of a given flag.
     * @param the flag's mask
     * @return the flag's value
     */
    bool getFlag( uint8_t mask );
    
    /**
     * Sets the half carry flag based on the previous and current values
     * of an 8-bit value.
     * @param prev the previous value
     * @param delta the change to prev
     * @param subtract whether or not this is being performed on a subtract operation (DEFAULT: FALSE)
     * @param offset for adc/sbc (DEFAULT: 0)
     */
    void setHalfCarry( uint8_t prev, uint8_t delta, bool subtract=false, int offset=0 );
    
    /**
     * Sets the carry flag based on the previous and current values
     * of an 8-bit value.
     * @param prev the previous value
     * @param delta the change to prev
     * @param subtract whether or not this was a subtract operation (DEFAULT: false)
     * @param offset for adc/sbc (DEFAULT: 0)
     */
    void setCarry( uint8_t prev, uint8_t delta, bool subtract=false, int offset=0 );

    /**
     * Fetches the next instruction and increments the program counter.
     * @return the instruction's opcode
     */
    uint8_t fetchNextInstruction( void );

    /**
     * Executes a CB-prefixed instruction.
     * @param opcode the instruction to execute
     * @return the number of ticks this instruction took
     */
    uint8_t executeCBInstruction( uint8_t opcode );

    /**
     * Executes a single byte load command. These fall between
     * 0x40 and 0x7F.
     * @param opcode the instruction to execute
     * @return the number of ticks this instruction took
     */
    uint8_t executeGroup4x( uint8_t opcode );

    /**
     * Executes an add instruction on the A register. These fall between
     * 0x80 and 0x8F.
     * @param opcode the instruction to execute
     * @return the number of ticks this instruction took
     */
    uint8_t executeGroup8x( uint8_t opcode );

    /**
     * Executes a sub instruction on the A register. These fall between
     * 0x90 and 0x9F.
     * @param opcode the instruction to execute
     * @return the number of ticks this instruction took
     */
    uint8_t executeGroup9x( uint8_t opcode );

    /**
     * Executes an AND instruction on the A register. These fall between
     * 0xA0 and 0xA7.
     * @param opcode the instruction to execute
     * @return the number of ticks this instruction took
     */
    uint8_t executeGroupAx0( uint8_t opcode );

    /**
     * Executes an XOR instruction on the A register. These fall between
     * 0xA8 and 0xAF.
     * @param opcode the instruction to execute
     * @return the number of ticks this instruction took
     */
    uint8_t executeGroupAx8( uint8_t opcode );    
    
    /**
     * Executes an OR instruction on the A register. These fall between
     * 0xB0 and 0xB7.
     * @param opcode the instruction to execute
     * @return the number of ticks this instruction took
     */
    uint8_t executeGroupBx0( uint8_t opcode );    

    /**
     * Executes an CP instruction on the A register. These fall between
     * 0xB8 and 0xBF. This also includes opcode 0xFE.
     * @param opcode the instruction to execute
     * @return the number of ticks this instruction took
     */
    uint8_t executeGroupBx8( uint8_t opcode );

    /**
     * Adds a two-byte word to HL.
     * @param high the most significant byte
     * @param low the least significant byte
     * @return the number of ticks this addition took
     */
    uint8_t addToHL( uint8_t high, uint8_t low );

    /**
     * Adds a two-byte word to HL.
     * @param val the value to add
     * @return the number of ticks this addition took
     */    
    uint8_t addToHL( uint16_t val );
    
    /**
     * Sets the system interrupt flag. (0xFFFF)
     * @param enable whether the flag should be enabled or disabled
     * @return the number of ticks this instruction took
     */
    uint8_t enableInterrupt( bool enable );

    /**
     * Pushes a two-byte word to the stack.
     * @param word the word to push
     * @return the number of ticks this push took
     */
    uint8_t pushWord( uint16_t word );
    
    /**
     * Pushes a two-byte word to the stack.
     * @param high the most significant byte
     * @param low the least significant byte
     * @return the number of ticks this push took
     */
    uint8_t pushWord( uint8_t high, uint8_t low );

    /**
     * Pops a two-byte word from the stack.
     * @param high the most significant byte
     * @param low the least significant byte
     * @return the number of ticks this pop took
     */
    uint8_t popWord( uint8_t& high, uint8_t& low );

    /**
     * Relative jump.
     * @param flag whether or not to perform the jump (DEFAULT: true)
     * @return the number of ticks this jump took
     */
    uint8_t jrr8( bool flag = true );
    
    /**
     * Calls the immediate 16-bit address.
     * @param flag whether or not to perform this call (DEFAULT: true)
     * @return the number of ticks this call took
     */
    uint8_t callA16( bool flag = true );
    
    /**
     * Return from a call.
     * @return the number of ticks this ret took
     */
    uint8_t ret( void );

    /**
     * Return from a call and enable interrupts.
     * @return the number of ticks this instruction took
     */
    uint8_t reti( void );
    
    /**
     * If the flag is set to a value, return.
     * @param flag the flag to test
     * @param flagValue the value to test
     * @return the number of ticks this ret took
     */
    uint8_t retFlag( uint8_t flag, bool flagValue );
    
    /**
     * Rotates the A register left through the carry flag.
     * @return the number of ticks this rotation took
     */
    uint8_t rla( void );

    /**
     * Rotates the A register left.
     * @return the number of ticks this rotation took
     */
    uint8_t rlca( void );

    /**
     * Rotates the A register left through the carry flag.
     * @return the number of ticks this rotation took
     */
    uint8_t rra( void );
    
    /**
     * Rotates the A register right.
     * @return the number of ticks this rotation took
     */
    uint8_t rrca( void );
    
    /**
     * Decrements the value of a register by one.
     * @param reg the register to decrement
     * @return the number of ticks this decrement took
     */
    uint8_t decReg( uint8_t& reg );

    /**
     * Decrements the value of a 2-byte register by one.
     * @param high the high byte
     * @param low the low byte
     * @return the number of ticks this decrement took
     */
    uint8_t decReg( uint8_t& high, uint8_t& low );

    /**
     * Decrements the value of a 2-byte register by one.
     * @param reg the register to decrement
     * @return the number of ticks this decrement took
     */
    uint8_t decReg( uint16_t& reg );
    
    /**
     * Increments the value of a register by one.
     * @param reg the register to increment
     * @return the number of ticks this increment took
     */
    uint8_t incReg( uint8_t& reg );    

    /**
     * Increments the value POINTED TO by HL.
     * @return the number of ticks this increment took
     */
    uint8_t incHL( void );

    /**
     * Decrements the value POINTED TO by HL.
     * @return the number of ticks this increment took
     */
    uint8_t decHL( void );
    
    /**
     * Increments the value of a 2-byte register by one.
     * @param high the high byte
     * @param low the low byte
     * @return the number of ticks this increment took
     */
    uint8_t incReg( uint8_t& high, uint8_t& low );

    /**
     * Increments the value of a 2-byte register by one.
     * @param reg the register to increment
     * @return the number of ticks this increment took
     */
    uint8_t incReg( uint16_t& reg );

    /**
     * Loads the stack pointer to the address pointed to by A16.
     * into the stack pointer.
     * @return the number of ticks this load took
     */
    uint8_t loadSPToA16( void );
    
    /**
     * Loads the value pointed to by a 2-byte register to A.
     * @param high the high byte
     * @param low the low byte
     * @param hlOffset the offset to add to HL (DEFAULT: 0)
     * @return the number of ticks this load took
     */
    uint8_t loadToA( uint8_t high, uint8_t low, uint16_t hlOffset=0 );

    /**
     * Loads the immediate 2-byte register to A.
     * @return the number of ticks this load took
     */ 
    uint8_t loadA16ToA( void );

    /**
     * Loads the value of A to the address pointed to by 2-byte register.
     * @param high the high byte
     * @param low the low byte
     * @param offset the offset to add to the address (DEFAULT: 0)
     * @return the number of ticks this load took
     */
    uint8_t loadFromA( uint8_t high, uint8_t low, uint16_t hlOffset=0 );

    /**
     * Loads the value of A to the address pointed to by the immediate
     * 2-byte register.
     * @return the number of ticks this load took
     */
    uint8_t loadA16FromA( void );

    /**
     * Loads the immediate 8 bit value.
     * @return the immediate 8 bit value
     */ 
    uint8_t loadImm8( void );
    
    /**
     * Loads the immediate 8 bit value into a register.
     * @param reg the register into which the value is loaded
     * @return the number of ticks this load took
     */
    uint8_t loadImm8( uint8_t& reg );

    /**
     * Loads the immediate 8 bit value into the address pointed to by HL.
     * @return the number of ticks this load took
     */
    uint8_t loadImm8ToHL( void );
    
    /**
     * Loads the immediate 16 bit value into a 2-byte register.
     * @param high the high byte
     * @param low the low byte
     * @return the number of ticks this load took took
     */
    uint8_t loadImm16( uint8_t& high, uint8_t& low );

    /**
     * Loads the immediate 16 bit value into a 2-byte register.
     * @param reg the 2-byte register
     * @return the number of ticks this load took took
     */
    uint8_t loadImm16( uint16_t& reg );

    /**
     * Performs a restart.
     * @param loc the location to jump to
     * @return the number of ticks this restart took
     */
    uint8_t rst( uint8_t loc );
    
    /**
     * Jumps to the next immediate 16 bit address.
     * @param flag whether or not to perform the jump (DEFAULT: true)
     * @return the number of ticks this instruction took
     */
    uint8_t jpa16( bool flag = true );

    /**
     * Performs the DAA instruction.
     * @return the number of ticks this instruction took
     */
    uint8_t daa( void );

    /**
     * Adds immediate 8-bit data to the stack pointer.
     * @return the number of ticks this instruction took
     */
    uint8_t addImm8ToSP( void );

    /**
     * Loads the value of SP plus the immediate 8 bit value to HL.
     * @return the number of ticks this instruction took
     */
    uint8_t loadSPImm8ToHL( void );

    /**
     * Performs an RL operation.
     * @param reg the register to perform this on
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t rl( uint8_t& reg, bool hl = false );

    /**
     * Performs an RR operation.
     * @param reg the register to perform this on
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t rr( uint8_t& reg, bool hl = false );
    
    
    /**
     * Performs an RLC operation.
     * @param reg the register to perform this on
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t rlc( uint8_t& reg, bool hl = false );

    /**
     * Performs an RRC operation.
     * @param reg the register to perform this on
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t rrc( uint8_t& reg, bool hl = false );

    /**
     * Performs an SLA operation.
     * @param reg the register to perform this on
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t sla( uint8_t& reg, bool hl = false );

    /**
     * Performs an SRA operation.
     * @param reg the register to perform this on
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t sra( uint8_t& reg, bool hl = false );

    /**
     * Swaps the first and seconds numbles of a register.
     * @param reg the register to perform this on
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t swap( uint8_t& reg, bool hl = false );

    /**
     * Performs an SRL operation.
     * @param reg the register to perform this on
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t srl( uint8_t& reg, bool hl = false );

    /**
     * Performs a BIT operation.
     * @param reg the register to perform this on
     * @param bit the bit to test
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t bit( uint8_t& reg, uint8_t bit, bool hl = false );

    /**
     * Performs a RES operation.
     * @param reg the register to perform this on
     * @param bit the bit to test
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t res( uint8_t& reg, uint8_t bit, bool hl = false );

    /**
     * Performs a SET operation.
     * @param reg the register to perform this on
     * @param bit the bit to test
     * @param hl flag to perform this operation on (HL) (DEFAULT: false)
     * @return the number of ticks this instruction took
     */
    uint8_t set( uint8_t& reg, uint8_t bit, bool hl = false );
 
    
    /**
     * Calculates the value of a 2-byte register.
     * @param high the high byte
     * @param low the low byte
     * @return the value of the register
     */
    uint16_t get2ByteRegValue( uint8_t high, uint8_t low );
    
    /**
     * Separates a 2-byte register into its two constituent parts.
     * @param high the high byte
     * @param low the low byte
     * @param val the value to split
     */
    void split2ByteRegValue( uint8_t& high, uint8_t& low, uint16_t val );
    
    Bus* mp_bus;

    // halting flag
    bool m_halting;
    
    // CPU registers
    uint16_t m_pc, m_sp;
    uint8_t m_a, m_b, m_c, m_d, m_e, m_h, m_l;
    uint8_t m_flags;
    bool m_ime;
    
    // flag masks
    const uint8_t c_zero = 0x80;
    const uint8_t c_sub = 0x40;
    const uint8_t c_halfCarry = 0x20;
    const uint8_t c_carry = 0x10;    
};
