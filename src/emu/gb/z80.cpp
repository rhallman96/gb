#include "z80.h"
#include "bus.h"

#include <iostream>

Z80::Z80( Bus* bus ) :
    mp_bus( bus ),
    m_halting( false ),
    m_pc( 0x100 ),
    m_sp( 0xFFFE ),
    m_a( 0x01 ),
    m_b( 0x00 ),
    m_c( 0x13 ),
    m_d( 0x00 ),
    m_e( 0xD8 ),
    m_h( 0x01 ),
    m_l( 0x4D ),
    m_flags( 0xB0 ),
    m_ime( true )
{
    // Initialize memory to post-startup values    
    mp_bus->write( 0xFF05, 0x00 );
    mp_bus->write( 0xFF06, 0x00 );
    mp_bus->write( 0xFF07, 0x00 );
    mp_bus->write( 0xFF10, 0x80 );
    mp_bus->write( 0xFF11, 0xBF );
    mp_bus->write( 0xFF12, 0xF3 );
    mp_bus->write( 0xFF14, 0xBF );
    mp_bus->write( 0xFF16, 0x3F );
    mp_bus->write( 0xFF17, 0x00 );
    mp_bus->write( 0xFF19, 0xBF );
    mp_bus->write( 0xFF1A, 0x7F );
    mp_bus->write( 0xFF1B, 0xFF );
    mp_bus->write( 0xFF1C, 0x9F );
    mp_bus->write( 0xFF1E, 0xBF );
    mp_bus->write( 0xFF20, 0xFF );
    mp_bus->write( 0xFF21, 0x00 );
    mp_bus->write( 0xFF22, 0x00 );
    mp_bus->write( 0xFF23, 0xBF );
    mp_bus->write( 0xFF24, 0x77 );
    mp_bus->write( 0xFF25, 0xF3 );
    mp_bus->write( 0xFF26, 0xF1 );
    mp_bus->write( 0xFF40, 0x91 );
    mp_bus->write( 0xFF42, 0x00 );
    mp_bus->write( 0xFF43, 0x00 );
    mp_bus->write( 0xFF45, 0x00 );
    mp_bus->write( 0xFF47, 0xFC );
    mp_bus->write( 0xFF48, 0xFF );
    mp_bus->write( 0xFF49, 0xFF );
    mp_bus->write( 0xFF4A, 0x00 );
    mp_bus->write( 0xFF4B, 0x00 );
    mp_bus->write( 0xFFFF, 0x00 );
}

Z80::~Z80( void )
{
}

uint16_t Z80::getPC( void )
{
    return m_pc;
}

void Z80::printStatus( void )
{
    using namespace std;

    cout << "AF: " << hex << (int)(this->get2ByteRegValue( m_a, m_flags )) << endl
	 << "BC: " << hex << (int)(this->get2ByteRegValue( m_b, m_c )) << endl
	 << "DE: " << hex << (int)(this->get2ByteRegValue( m_d, m_e )) << endl
	 << "HL: " << hex << (int)(this->get2ByteRegValue( m_h, m_l )) << endl
	 << "SP: " << hex << (int)(m_sp) << endl
	 << "PC: " << hex << (int)(m_pc) << endl
	 << endl;
}

uint8_t Z80::executeNextInstruction( void )
{
    uint8_t ticks = 4;
    uint8_t opcode = 0x00;

    using std::cout;
    using std::hex;
    using std::endl;
    
    // only fetch next instruction if not halting
    if( m_halting == false ) { opcode = this->fetchNextInstruction(); }
    else
    {
	// check for interrupts
	this->checkInterrupts();
	return 4;
    }

    if( opcode == 0xCB )
    {
	// CB-prefixed opcodes
	loadImm8( opcode );
	ticks = this->executeCBInstruction( opcode );
    }
    else if( (opcode >= 0x40) && (opcode <= 0x7F) &&
	(opcode != 0x76 ) )
    {
	ticks = this->executeGroup4x( opcode );
    }
    else if( ((opcode >= 0x80) && (opcode <= 0x8F)) ||
	(opcode == 0xC6) ||
	(opcode == 0xCE) )
    {
	ticks = this->executeGroup8x( opcode );
    }
    else if( ((opcode >= 0x90) && (opcode <= 0x9F)) ||
	(opcode == 0xD6) ||
	(opcode == 0xDE) )
    {
	ticks = this->executeGroup9x( opcode );
    }
    else if( ((opcode >= 0xA0) && (opcode <= 0xA7)) ||
	(opcode == 0xE6) )
    {
	ticks = this->executeGroupAx0( opcode );
    }
    else if( ((opcode >= 0xA8) && (opcode <= 0xAF)) ||
	(opcode == 0xEE) )
    {
	ticks = this->executeGroupAx8( opcode );
    }
    else if( ((opcode >= 0xB0) && (opcode <= 0xB7)) ||
	(opcode == 0xF6) )
    {
	ticks = this->executeGroupBx0( opcode );
    }
    else if( ((opcode >= 0xB8) && (opcode <= 0xBF)) ||
	(opcode == 0xFE) )
    {
	ticks = this->executeGroupBx8( opcode );
    }
    else
    {
	switch( opcode )
	{
	case 0x01: // LD BC,d16
	    ticks = this->loadImm16( m_b, m_c );
	    break;
	case 0x02: // LD (BC),A
	    ticks = this->loadFromA( m_b, m_c );
	    break;
	case 0x03: // INC BC
	    ticks = this->incReg( m_b, m_c );
	    break;
	case 0x04: // INC B
	    ticks = this->incReg( m_b );
	    break;
	case 0x05: // DEC B
	    ticks = this->decReg( m_b );
	    break;
	case 0x06: // LD B,d8
	    ticks = this->loadImm8( m_b );
	    break;
	case 0x07: // RLCA
	    ticks = this->rlca();
	    break;
	case 0x08: // LD (a16),SP
	    ticks = this->loadSPToA16();
	    break;
	case 0x09: // ADD HL,BC
	    ticks = this->addToHL( m_b, m_c );
	    break;
	case 0x0A: // LD A,(BC)
	    ticks = this->loadToA( m_b, m_c );
	    break;
	case 0x0B: // DEC BC
	    ticks = this->decReg( m_b, m_c );
	    break;
	case 0x0C: // INC C
	    ticks = this->incReg( m_c ); 
	    break;
	case 0x0D: // DEC C
	    ticks = this->decReg( m_c );
	    break;
	case 0x0E: // LD C,d8
	    ticks = this->loadImm8( m_c );
	    break;
	case 0x0F: // RRCA
	    ticks = this->rrca();
	    break;
	case 0x10: // STOP 0
	    // TODO: unimplemented
	    break;
	case 0x11: // LD DE,d16
	    ticks = this->loadImm16( m_d, m_e );
	    break;
	case 0x12: // LD (DE),A
	    ticks = this->loadFromA( m_d, m_e );
	    break;
	case 0x13: // INC DE
	    ticks = this->incReg( m_d, m_e );
	    break;
	case 0x14: // INC D
	    ticks = this->incReg( m_d );
	    break;
	case 0x15: // DEC D
	    ticks = this->decReg( m_d );
	    break;
	case 0x16: // LD D,d8
	    ticks = this->loadImm8( m_d );
	    break;
	case 0x17: // RLA
	    ticks = this->rla();
	    break;
	case 0x18: // JR r8
	    ticks = this->jrr8();
	    break;
	case 0x19: // ADD HL,DE
	    ticks = this->addToHL( m_d, m_e );
	    break;
	case 0x1A: // LD A,(DE)
	    ticks = this->loadToA( m_d, m_e );
	    break;
	case 0x1B: // DEC DE
	    ticks = this->decReg( m_d, m_e );
	    break;
	case 0x1C: // INC E
	    ticks = this->incReg( m_e );
	    break;
	case 0x1D: // DEC E
	    ticks = this->decReg( m_e );
	    break;
	case 0x1E: // LD E,d8
	    ticks = this->loadImm8( m_e );
	    break;
	case 0x1F: // RRA
	    ticks = this->rra();
	    break;
	case 0x20: // JR NZ,r8
	    ticks = this->jrr8( !(this->getFlag(c_zero)) );
	    break;
	case 0x21: // LD HL,d16
	    ticks = this->loadImm16( m_h, m_l );
	    break;
	case 0x22: // LD (HL+),A
	    ticks = this->loadFromA( m_h, m_l, 1 );
	    break;
	case 0x23: // INC HL
	    ticks = this->incReg( m_h, m_l );
	    break;
	case 0x24: // INC H
	    ticks = this->incReg( m_h );
	    break;
	case 0x25: // DEC H
	    ticks = this->decReg( m_h );
	    break;
	case 0x26: /// LD H,d8
	    ticks = this->loadImm8( m_h );
	    break;
	case 0x27: // DAA
	    ticks = this->daa();
	    break;
	case 0x28: // JR Z,r8
	    ticks = this->jrr8( this->getFlag( c_zero ) );
	    break;
	case 0x29: // ADD HL,HL
	    ticks = this->addToHL( m_h, m_l );
	    break;
	case 0x2A: // LD A,(HL+)
	    ticks = this->loadToA( m_h, m_l, 1 );
	    break;
	case 0x2B: // DEC HL
	    ticks = this->decReg( m_h, m_l );
	    break;
	case 0x2C: // INC L
	    ticks = this->incReg( m_l );
	    break;
	case 0x2D: // DEC L
	    ticks = this->decReg( m_l );
	    break;
	case 0x2E: // LD L,d8
	    ticks = this->loadImm8( m_l );
	    break;
	case 0x2F: // CPL
	    m_a = ~m_a;
	    this->setFlag( c_sub );
	    this->setFlag( c_halfCarry );
	    ticks = 4;
	    break;
	case 0x30: // JR NC,r8
	    ticks = this->jrr8( !(this->getFlag(c_carry)) );
	    break;
	case 0x31: // LD SP,d16
	    ticks = this->loadImm16( m_sp );
	    break;
	case 0x32: // LD (HL-),A
	    ticks = this->loadFromA( m_h, m_l, 0xFFFF );
	    break;
	case 0x33: // INC SP
	    ticks = this->incReg( m_sp );
	    break;
	case 0x34: // INC (HL)
	    ticks = this->incHL();
	    break;
	case 0x35: // DEC (HL)
	    ticks = this->decHL();
	    break;
	case 0x36: // LD (HL),d8
	    ticks = this->loadImm8ToHL();
	    break;
	case 0x37: // SCF
	    this->setFlag( c_carry );
	    this->setFlag( c_sub, false );
	    this->setFlag( c_halfCarry, false );
	    ticks = 4;
	    break;
	case 0x38: // JR C,r8
	    ticks = this->jrr8( this->getFlag( c_carry ));
	    break;
	case 0x39: // ADD HL,SP
	    ticks = this->addToHL( m_sp );
	    break;
	case 0x3A: // LD A,(HL-)
	    ticks = this->loadToA( m_h, m_l, 0xFFFF );
	    break;
	case 0x3B: // DEC SP
	    ticks = this->decReg( m_sp );
	    break;
	case 0x3C: // INC A
	    ticks = this->incReg( m_a );
	    break;
	case 0x3D: // DEC A
	    ticks = this->decReg( m_a );
	    break;
	case 0x3E: // LD A,d8
	    ticks = this->loadImm8( m_a );
	    break;
	case 0x3F: // CCF
	    this->setFlag( c_carry, !(this->getFlag(c_carry)) );
	    this->setFlag( c_sub, false );
	    this->setFlag( c_halfCarry, false );
	    ticks = 4;
	    break;
	case 0x76: // HALT 
	    ticks = this->halt();
	    break;
	case 0xC0: // RET NZ
	    ticks = this->retFlag( c_zero, false );
	    break;
	case 0xC1: // POP BC
	    ticks = this->popWord( m_b, m_c );
	    break;
	case 0xC2: // JP NZ,a16
	    ticks = this->jpa16( !(this->getFlag(c_zero)) );
	    break;
	case 0xC3: // JP a16
	    ticks = this->jpa16();
	    break;
	case 0xC4: // CALL NZ,a16
	    ticks = this->callA16( !(this->getFlag(c_zero)) );
	    break;
	case 0xC5: // PUSH BC
	    ticks = this->pushWord( m_b, m_c );
	    break;
	case 0xC7: // RST 00H
	    ticks = this->rst( 0x00 );
	    break;
	case 0xC8: // RET Z
	    ticks = this->retFlag( c_zero, true );
	    break;
	case 0xC9: // RET
	    ticks = this->ret();
	    break;
	case 0xCA: // JP Z,a16
	    ticks = this->jpa16( this->getFlag( c_zero ) );
	    break;
	case 0xCC: // CALL Z,a16
	    ticks = this->callA16( this->getFlag( c_zero ) );
	    break;
	case 0xCD: // CALL a16
	    ticks = this->callA16();
	    break;
	case 0xCF: // RST 08H
	    ticks = this->rst( 0x08 );
	    break;
	case 0xD0: // RET NC
	    ticks = this->retFlag( c_carry, false );
	    break;
	case 0xD1: // POP DE
	    ticks = this->popWord( m_d, m_e );
	    break;
	case 0xD2: // JP NC,a16
	    ticks = this->jpa16( !(this->getFlag(c_carry)) );
	    break;
	case 0xD4: // CALL NC,a16
	    ticks = this->callA16( !(this->getFlag(c_carry)) );
	    break;
	case 0xD5: // PUSH DE
	    ticks = this->pushWord( m_d, m_e );
	    break;
	case 0xD7: // RST 10H
	    ticks = this->rst( 0x10 );
	    break;
	case 0xD8: // RET C
	    ticks = this->retFlag( c_carry, true );
	    break;
	case 0xD9: // RETI
	    ticks = this->reti();
	    break;
	case 0xDA: // JP C,a16
	    ticks = this->jpa16( this->getFlag( c_carry ) );
	    break;
	case 0xDC: // CALL C,a16
	    ticks = this->callA16( this->getFlag( c_carry ) );
	    break;
	case 0xDF: // RST 18H
	    ticks = this->rst( 0x18 );
	    break;
	case 0xE0: // LDH (a8),A
	    this->loadFromA( 0xFF, this->loadImm8() );
	    ticks = 12;
	    break;
	case 0xE1: // POP HL
	    ticks = this->popWord( m_h, m_l );
	    break;
	case 0xE2: // LD (C),A
	    ticks = this->loadFromA( 0xFF, m_c );
	    break;
	case 0xE5: // PUSH HL
	    ticks = this->pushWord( m_h, m_l );
	    break;
	case 0xE7: // RST 20H
	    ticks = this->rst( 0x20 );
	    break;
	case 0xE8: // ADD SP,r8
	    ticks = this->addImm8ToSP();
	    break;
	case 0xE9: // JP (HL)
	    m_pc = this->get2ByteRegValue( m_h, m_l ) - 1;
	    ticks = 4;
	    break;
	case 0xEA: // LD (a16),A
	    ticks = this->loadA16FromA();
	    break;
	case 0xEF: // RST 28H
	    ticks = this->rst( 0x28 );
	    break;
	case 0xF0: // LDH A,(a8)
	    this->loadToA( 0xFF, this->loadImm8() );
	    ticks = 12;
	    break;
	case 0xF1: // POP AF
	    ticks = this->popWord( m_a, m_flags );
	    m_flags = m_flags & 0xF0; // lower four bits are always zero
	    break;
	case 0xF2: // LD A,(C)
	    ticks = this->loadToA( 0xFF, m_c );
	    break;
	case 0xF3: // DI
	    ticks = this->enableInterrupt( false );
	    break;
	case 0xF5: // PUSH AF
	    ticks = pushWord( m_a, m_flags );
	    break;
	case 0xF7: // RST 30H
	    ticks = this->rst( 0x30 );
	    break;
	case 0xF8: // LD HL,SP+r8
	    ticks = this->loadSPImm8ToHL();
	    break;
	case 0xF9: // LD SP,HL
	    m_sp = this->get2ByteRegValue( m_h, m_l );
	    ticks = 8;
	    break;
	case 0xFA: // LD A,(a16)
	    ticks = this->loadA16ToA();
	    break;
	case 0xFB: // EI
	    ticks = this->enableInterrupt( true );
	    break;
	case 0xFF: // RST 38H
	    ticks = this->rst( 0x38 );
	    break;
	case 0x00: // NOP
	default:
	    break;
	}
    }

    // after executing, increment program counter
    m_pc++;
    
    // check for interrupts
    this->checkInterrupts();
    
    return ticks;
}

void Z80::triggerInterrupt( uint8_t mask )
{
    uint8_t ifreg;
    mp_bus->access( 0xFF0F, ifreg, READ );
    ifreg = ifreg | mask;
    mp_bus->access( 0xFF0F, ifreg, WRITE );
}

uint8_t Z80::halt( void )
{
    if( m_ime == true )
    {
	m_halting = true;
    }
    else
    {
	uint8_t ieReg, ifReg;
	mp_bus->defaultAccess( 0xFFFF, ieReg, READ );
	mp_bus->defaultAccess( 0xFF0F, ifReg, READ );

	if( (ieReg & ifReg) == 0 )
	{
	    m_halting = true;
	}
    }
    
    return 4;
}

void Z80::checkInterrupts( void )
{
    uint8_t ieReg, ifReg;
    mp_bus->defaultAccess( 0xFFFF, ieReg, READ );
    mp_bus->defaultAccess( 0xFF0F, ifReg, READ );

    if( (ieReg & ifReg) == 0x00 ) {
	return;
    }

    // if an interrupt exists, no longer halting
    m_halting = false;

    // if the IME flag is not set, don't process the interrupt
    if( m_ime == false ) {
	return;
    }
    
    this->enableInterrupt( false );
    this->pushWord( m_pc );
    
    if( ( ieReg & c_vBlank ) &&
	( ifReg & c_vBlank ) )
    {
	// VBLANK
	ifReg = ifReg & (~c_vBlank);
	m_pc = 0x40;
    }
    else if( ( ieReg & c_LCDStat ) &&
	     ( ifReg & c_LCDStat ) )
    {
	// LCD STAT
	ifReg = ifReg & (~c_LCDStat);
	m_pc = 0x48;
    }
    else if( ( ieReg & c_timer ) &&
	     ( ifReg & c_timer ) )
    {
	// TIMER
	ifReg = ifReg & (~c_timer);
	m_pc = 0x50;
    }
    else if( ( ieReg & c_serial ) &&
	     ( ifReg & c_serial ) )
    {
	// SERIAL
	ifReg = ifReg & (~c_serial);
	m_pc = 0x58;
    }
    else if( ( ieReg & c_joypad ) &&
	     ( ifReg & c_joypad ) )
    {
	// JOYPAD
	ifReg = ifReg & (~c_joypad);
	m_pc = 0x60;
    }
    
    mp_bus->access( 0xFF0F, ifReg, WRITE );    
}

uint8_t Z80::executeGroup4x( uint8_t opcode )
{
    uint8_t ticks = 4;
    
    uint8_t high = opcode >> 4;
    uint8_t low = opcode & 0x0F;
    uint8_t value;

    // determine value to store in the register
    switch( low )
    {
    case 0x00:
    case 0x08:
	value = m_b;
	break;
    case 0x01:
    case 0x09:
	value = m_c;
	break;
    case 0x02:
    case 0x0A:
	value = m_d;
	break;
    case 0x03:
    case 0x0B:
	value = m_e;
	break;
    case 0x04:
    case 0x0C:
	value = m_h;
	break;
    case 0x05:
    case 0x0D:
	value = m_l;
	break;
    case 0x06:
    case 0x0E:
        {
	    uint16_t hl = get2ByteRegValue( m_h, m_l );
	    mp_bus->access( hl, value, READ );
	    ticks = 8;
	}
	break;
    case 0x07:
    case 0x0F:
	value = m_a;
	break;
    }

    // determine the register in which to store the value
    switch( high )
    {
    case 0x04:
	if( low < 8 ) { m_b = value; }
	else { m_c = value; }
	break;
    case 0x05:
	if( low < 8 ) { m_d = value; }
	else { m_e = value; }
	break;
    case 0x06:
	if( low < 8 ) { m_h = value; }
	else { m_l = value; }
	break;
    case 0x07:
	if( low < 8 )
	{
	    uint16_t hl = get2ByteRegValue( m_h, m_l );
	    mp_bus->access( hl, value, WRITE );
	    ticks = 8;
	}
	else { m_a = value; }
    default:
	break;
    }

    return ticks;
}

uint8_t Z80::executeGroup8x( uint8_t opcode )
{
    uint8_t ticks = 4;
    uint8_t value;
    
    uint8_t low = opcode & 0x0F;
    int carry = 0;

    if( (low > 0x07) && this->getFlag( c_carry ) )
    {
	carry = 1;
    }
    
    if( (opcode == 0xC6) || (opcode == 0xCE) )
    {
	this->loadImm8( value );
	ticks = 8;
    }
    else
    {
	switch( low )
	{
	case 0x00:
	case 0x08:
	    value = m_b;
	    break;
	case 0x01:
	case 0x09:
	    value = m_c;
	    break;
	case 0x02:
	case 0x0A:
	    value = m_d;
	    break;
	case 0x03:
	case 0x0B:
	    value = m_e;
	    break;
	case 0x04:
	case 0x0C:
	    value = m_h;
	    break;
	case 0x05:
	case 0x0D:
	    value = m_l;
	    break;
	case 0x06:
	case 0x0E:
        {
	    uint16_t hl = get2ByteRegValue( m_h, m_l );
	    mp_bus->access( hl, value, READ );
	    ticks = 8;
        }
	break;
	case 0x07:
	case 0x0F:
	    value = m_a;
	    break;
	}
    }
    
    uint8_t prev = m_a;
    m_a = m_a + value + carry;

    // Set flags
    this->setFlag( c_zero, (m_a == 0) );
    this->setFlag( c_sub, false );
    this->setCarry( prev, value, false, carry );
    this->setHalfCarry( prev, value, false, carry );
    
    return ticks;
}

uint8_t Z80::executeGroup9x( uint8_t opcode )
{    
    uint8_t ticks = 4;
    
    uint8_t low = opcode & 0x0F;
    int carry = 0;
    if( (low > 0x07) && this->getFlag( c_carry ) )
    {
	carry = 1;
    }

    uint8_t value;

    if( (opcode == 0xD6) || (opcode == 0xDE) )
    {
	this->loadImm8( value );
	ticks = 8;
    }
    else
    {
	switch( low )
	{
	case 0x00:
	case 0x08:
	    value = m_b;
	    break;
	case 0x01:
	case 0x09:
	    value = m_c;
	    break;
	case 0x02:
	case 0x0A:
	    value = m_d;
	    break;
	case 0x03:
	case 0x0B:
	    value = m_e;
	    break;
	case 0x04:
	case 0x0C:
	    value = m_h;
	    break;
	case 0x05:
	case 0x0D:
	    value = m_l;
	    break;
	case 0x06:
	case 0x0E:
        {
	    uint16_t hl = get2ByteRegValue( m_h, m_l );
	    mp_bus->access( hl, value, READ );
	    ticks = 8;
        }
	break;
	case 0x07:
	case 0x0F:
	    value = m_a;
	    break;
	}
    }
    
    uint8_t prev = m_a;
    m_a = m_a - value - carry;
    
    // Set flags
    this->setFlag( c_zero, (m_a == 0) );
    this->setFlag( c_sub );
    this->setHalfCarry( prev, value, true, carry );
    this->setCarry( prev, value, true, carry );

    return ticks;
}

uint8_t Z80::executeGroupAx0( uint8_t opcode )
{
    uint8_t ticks = 4;    
    uint8_t low = opcode & 0x0F;
    uint8_t value;

    if( opcode == 0xE6 )
    {
	this->loadImm8( value );
	ticks = 8;
    }
    else
    {
	switch( low )
	{
	case 0x00:
	    value = m_b;
	    break;
	case 0x01:
	    value = m_c;
	    break;
	case 0x02:
	    value = m_d;
	    break;
	case 0x03:
	    value = m_e;
	    break;
	case 0x04:
	    value = m_h;
	    break;
	case 0x05:
	    value = m_l;
	    break;
	case 0x06:
        {
	    uint16_t hl = get2ByteRegValue( m_h, m_l );
	    mp_bus->access( hl, value, READ );
	    ticks = 8;
        }
	break;
	case 0x07:
	default:
	    value = m_a;
	    break;
	}
    }
    
    m_a = m_a & value;

    // Set flags
    this->setFlag( c_zero, (m_a == 0) );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, true );
    this->setFlag( c_carry, false );

    return ticks;
}

uint8_t Z80::executeGroupAx8( uint8_t opcode )
{
    uint8_t ticks = 4;    
    uint8_t value;

    if( opcode == 0xEE )
    {
	this->loadImm8( value );
	ticks = 8;
    }
    else
    {
	uint8_t low = opcode & 0x0F;
	switch( low )
	{
	case 0x08:
	    value = m_b;
	    break;
	case 0x09:
	    value = m_c;
	    break;
	case 0x0A:
	    value = m_d;
	    break;
	case 0x0B:
	    value = m_e;
	    break;
	case 0x0C:
	    value = m_h;
	    break;
	case 0x0D:
	    value = m_l;
	    break;
	case 0x0E:
           {
	       uint16_t hl = get2ByteRegValue( m_h, m_l );
	       mp_bus->access( hl, value, READ );
	       ticks = 8;
	   }
	   break;
	case 0x0F:
	default:
	    value = m_a;
	    break;
	}
    }
    
    m_a = m_a ^ value;

    this->setFlag( c_zero, (m_a == 0) );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );
    this->setFlag( c_carry, false );

    return ticks;
}

uint8_t Z80::executeGroupBx0( uint8_t opcode )
{
    uint8_t ticks = 4;    
    uint8_t low = opcode & 0x0F;
    uint8_t value;

    if( opcode == 0xF6 )
    {
	value = this->loadImm8();
	ticks = 8;
    }
    else
    {
	switch( low )
	{
	case 0x00:
	    value = m_b;
	    break;
	case 0x01:
	    value = m_c;
	    break;
	case 0x02:
	    value = m_d;
	    break;
	case 0x03:
	    value = m_e;
	    break;
	case 0x04:
	    value = m_h;
	    break;
	case 0x05:
	    value = m_l;
	    break;
	case 0x06:
            {
		uint16_t hl = get2ByteRegValue( m_h, m_l );
		mp_bus->access( hl, value, READ );
		ticks = 8;
            }
	    break;
	case 0x07:
	    value = m_a;
	    break;
	}
    }
    
    m_a = m_a | value;

    // Set flags
    this->setFlag( c_zero, (m_a == 0) );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );
    this->setFlag( c_carry, false );

    return ticks;
}

uint8_t Z80::addToHL( uint16_t val )
{
    uint8_t high, low;
    this->split2ByteRegValue( high, low, val );
    return this->addToHL( high, low );
}

uint8_t Z80::addToHL( uint8_t high, uint8_t low )
{ 
    uint16_t hl = this->get2ByteRegValue( m_h, m_l );
    uint16_t val = this->get2ByteRegValue( high, low );
        
    uint16_t soln = hl + val;
    int next = (int)hl + (int)val;
    
    bool carry = ((next & 0x10000) != 0);
    bool halfCarry = (
	( ((int)hl & 0xFFF) +
	  ((int)val & 0xFFF) ) & 0x1000 ) != 0;
    
    split2ByteRegValue( m_h, m_l, soln );

    // Set flags
    this->setFlag( c_sub, false );
    this->setFlag( c_carry, carry );
    this->setFlag( c_halfCarry, halfCarry );
    
    return 8;
}

uint8_t Z80::executeGroupBx8( uint8_t opcode )
{
    uint8_t ticks = 4;    
    uint8_t low = opcode & 0x0F;
    uint8_t value;

    if( opcode == 0xFE )
    {
	this->loadImm8( value );
	ticks = 8;
    }
    else
    {
	switch( low )
	{
	case 0x08:
	    value = m_b;
	    break;
	case 0x09:
	    value = m_c;
	    break;
	case 0x0A:
	    value = m_d;
	    break;
	case 0x0B:
	    value = m_e;
	    break;
	case 0x0C:
	    value = m_h;
	    break;
	case 0x0D:
	    value = m_l;
	    break;
	case 0x0E:
        {
	    uint16_t hl = get2ByteRegValue( m_h, m_l );
	    mp_bus->access( hl, value, READ );
	    ticks = 8;
        }
	break;
	case 0x0F:
	    value = m_a;
	    break;
	}
    }

    uint8_t curr = m_a - value;
    
    // Set flags
    this->setFlag( c_zero, (curr == 0) );
    this->setFlag( c_sub );
    this->setHalfCarry( m_a, value, true );
    this->setCarry( m_a, value, true );
    
    return ticks;
}

uint8_t Z80::enableInterrupt( bool enable )
{
    m_ime = enable;
    return 4;
}

uint8_t Z80::pushWord( uint16_t word )
{
    uint8_t high, low;
    this->split2ByteRegValue( high, low, word );
    this->pushWord( high, low );
    return 16;
}

uint8_t Z80::pushWord( uint8_t high, uint8_t low )
{
    m_sp -= 2;
    mp_bus->access( m_sp + 1, high, WRITE );
    mp_bus->access( m_sp, low, WRITE );
    return 16;
}

uint8_t Z80::popWord( uint8_t& high, uint8_t& low )
{
    mp_bus->access( m_sp + 1, high, READ );
    mp_bus->access( m_sp, low, READ );
    m_sp += 2;
    
    return 12;
}

uint8_t Z80::jrr8( bool flag )
{
    if( flag == false )
    {
	m_pc++;
	return 8;
    }
        
    uint8_t r8 = this->loadImm8();

    bool sign = BIT( r8, 7 );
    uint8_t magnitude;

    if( sign )
    {
	magnitude = ~r8;
	magnitude++;
	m_pc -= magnitude;
    }
    else
    {
	magnitude = r8 & 0x7F;
	m_pc += magnitude;
    }
    
    return 12;
}

uint8_t Z80::callA16( bool flag )
{
    if( flag == false )
    {
	m_pc += 2;
	return 12;
    }
    
    uint16_t addr;
    uint8_t high, low;

    this->loadImm16( high, low );
    addr = this->get2ByteRegValue( high, low );
    this->pushWord( m_pc + 1 );

    m_pc = addr;
    m_pc--; // go to the address before the call
    
    
    return 24;
}

uint8_t Z80::ret( void )
{
    uint16_t addr;
    uint8_t high, low;
    this->popWord( high, low );
    addr = this->get2ByteRegValue( high, low );
    m_pc = addr - 1;

    return 16;
}

uint8_t Z80::reti( void )
{
    using namespace std;
    
    this->enableInterrupt( true );
    this->ret();
    
    return 16;
}

uint8_t Z80::retFlag( uint8_t flag, bool flagValue )
{
    bool value = this->getFlag( flag );
    if( value != flagValue ) { return 8; }

    this->ret();
    return 20;
}

uint8_t Z80::rla( void )
{
    bool oldCarry = this->getFlag( c_carry );
    bool nextCarry = BIT( m_a, 7 );

    m_a = m_a << 1;
    if( oldCarry ) { m_a |= 0x01; }
    
    // Set flags
    this->setFlag( c_zero, false );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );    
    this->setFlag( c_carry, nextCarry );
    
    return 4;
}

uint8_t Z80::rlca( void )
{
    bool nextCarry = BIT( m_a, 7 );

    m_a = m_a << 1;
    if( nextCarry ) { m_a |= 0x01; }
    
    // Set flags
    this->setFlag( c_zero, false );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );    
    this->setFlag( c_carry, nextCarry );

    return 4;
}

uint8_t Z80::rra( void )
{
    bool oldCarry = this->getFlag( c_carry );
    bool nextCarry = BIT( m_a, 0 );

    m_a = m_a >> 1;
    if( oldCarry ) { m_a |= 0x80; }
    
    // Set flags
    this->setFlag( c_zero, false );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );    
    this->setFlag( c_carry, nextCarry );
    
    return 4;
}

uint8_t Z80::rrca( void )
{
    bool nextCarry = BIT( m_a, 0 );

    m_a = m_a >> 1;
    if( nextCarry ) { m_a |= 0x80; }
    
    // Set flags
    this->setFlag( c_zero, false );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );    
    this->setFlag( c_carry, nextCarry );
    
    return 4;
}

uint8_t Z80::decReg( uint8_t& reg )
{
    uint8_t old = reg;
    reg--;

    // Set flags
    this->setFlag( c_zero, (reg == 0) );
    this->setFlag( c_sub );
    this->setHalfCarry( old, 1, true );    

    return 4;
}

uint8_t Z80::decReg( uint8_t& high, uint8_t& low )
{
    uint16_t temp = this->get2ByteRegValue( high, low ) - 1;
    this->split2ByteRegValue( high, low, temp );
    
    return 8;
}

uint8_t Z80::decReg( uint16_t& reg )
{
    reg--;
    return 8;
}

uint8_t Z80::loadSPToA16( void )
{
    uint16_t addr;
    uint8_t high, low;

    this->loadImm16( high, low );
    addr = this->get2ByteRegValue( high, low );

    uint8_t spHigh, spLow;
    this->split2ByteRegValue( spHigh, spLow, m_sp );

    mp_bus->access( addr, spLow, WRITE );
    mp_bus->access( addr + 1, spHigh, WRITE );
    
    return 20;
}

uint8_t Z80::incReg( uint8_t& reg )
{
    uint8_t old = reg;
    reg++;
    
    // Set flags
    this->setFlag( c_zero, (reg == 0) );
    this->setFlag( c_sub, false );
    this->setHalfCarry( old, 1 );
    
    return 4;
}

uint8_t Z80::incHL( void )
{
    uint16_t hl = this->get2ByteRegValue( m_h, m_l );
    uint8_t prev, val;

    mp_bus->access( hl, prev, READ );

    val = prev + 1;

    // Set flags
    this->setFlag( c_zero, (val == 0) );
    this->setFlag( c_sub, false );
    this->setHalfCarry( prev, 1 );

    mp_bus->access( hl, val, WRITE );
    
    return 12;
}

uint8_t Z80::decHL( void )
{
    uint16_t hl = this->get2ByteRegValue( m_h, m_l );
    uint8_t prev, val;

    mp_bus->access( hl, prev, READ );

    val = prev - 1;

    // Set flags
    this->setFlag( c_zero, (val == 0) );
    this->setFlag( c_sub );
    this->setHalfCarry( prev, 1, true );

    mp_bus->access( hl, val, WRITE );

    return 12;
}

uint8_t Z80::loadToA( uint8_t high, uint8_t low, uint16_t hlOffset )
{
    uint16_t addr = this->get2ByteRegValue( high, low );
    mp_bus->access( addr, m_a, READ );
    
    if( hlOffset != 0 )
    {
	addr += hlOffset;
        this->split2ByteRegValue( high, low, addr );
	m_h = high;
	m_l = low;
    }
    
    return 8;
}

uint8_t Z80::loadA16ToA( void )
{
    uint8_t high, low;
    this->loadImm16( high, low );
    this->loadToA( high, low );
    
    return 16;
}

uint8_t Z80::loadFromA( uint8_t high, uint8_t low, uint16_t hlOffset )
{
    using namespace std;
    
    uint16_t addr = this->get2ByteRegValue( high, low );

    mp_bus->access( addr, m_a, WRITE );
    uint8_t val;
    mp_bus->access( addr, val, READ );

    if( hlOffset != 0 )
    {
	addr += hlOffset;
        this->split2ByteRegValue( high, low, addr );
	m_h = high;
	m_l = low;
    }
    
    return 8;
}

uint8_t Z80::loadA16FromA( void )
{
    uint8_t high, low;
    this->loadImm16( high, low );    
    this->loadFromA( high, low );
    
    return 16;
}

uint8_t Z80::incReg( uint8_t& high, uint8_t& low )
{
    uint16_t temp = this->get2ByteRegValue( high, low );
    temp++;

    high = (temp >> 8);
    low = (temp & 0xFF);
    
    return 8;
}

uint8_t Z80::incReg( uint16_t& reg )
{
    reg++;
    return 8;
}

uint8_t Z80::loadImm8( void )
{
    uint8_t val;
    this->loadImm8( val );
    return val;
}

uint8_t Z80::loadImm8( uint8_t& reg )
{
    mp_bus->access( ++m_pc, reg, READ );
    return 8;
}

uint8_t Z80::loadImm8ToHL( void )
{
    uint16_t addr = this->get2ByteRegValue( m_h, m_l );
    uint8_t val;
    this->loadImm8( val );    
    mp_bus->access( addr, val, WRITE );
    
    return 12;
}

uint8_t Z80::loadImm16( uint8_t& high, uint8_t& low )
{
    mp_bus->access( ++m_pc, low, READ );
    mp_bus->access( ++m_pc, high, READ );
    return 12;
}

uint8_t Z80::loadImm16( uint16_t& reg )
{
    uint16_t value = 0;
    uint8_t high, low;
    
    this->loadImm16( high, low );
    value = this->get2ByteRegValue( high, low );
    
    reg = value;
    
    return 12;
}

uint8_t Z80::rst( uint8_t loc )
{
    this->pushWord( m_pc + 1 );
    m_pc = loc - 1;
    
    return 16;
}

uint8_t Z80::jpa16( bool flag )
{
    uint8_t high, low;
    this->loadImm16( high, low );
    
    if( flag == false )
    {
	return 12;
    }

    m_pc = this->get2ByteRegValue( high, low );
    m_pc--; // go to the address before the jump
    
    return 16;
}

uint8_t Z80::daa( void )
{
    uint8_t delta = 0;
    bool carry = false;

    if( this->getFlag( c_halfCarry) ||
	( (this->getFlag( c_sub ) == false) &&
	  ((m_a & 0x0F) > 9) ) )
    {
	// if half carry or lower nibble greater than nine, add 6
	delta += 0x06;
    }

    if( this->getFlag( c_carry ) ||
	( (this->getFlag( c_sub ) == false) &&
	  (m_a > 0x99) ) )
    {
	// if carry or a is greater than 0x99, add 0x60
	delta += 0x60;
	carry = true;
    }

    if( this->getFlag( c_sub ) ) { m_a -= delta; }
    else { m_a += delta; }
    
    // Set flags
    this->setFlag( c_zero, (m_a == 0x00) );
    this->setFlag( c_halfCarry, false );
    this->setFlag( c_carry, carry );
    
    return 4;
}

uint8_t Z80::loadSPImm8ToHL( void )
{
    uint16_t oldSP = m_sp;

    this->addImm8ToSP();
    this->split2ByteRegValue( m_h, m_l, m_sp );
    
    m_sp = oldSP;
    return 12;
}


uint8_t Z80::addImm8ToSP( void )
{
    bool halfCarry = false;
    bool carry = false;
    
    uint8_t r8;
    this->loadImm8( r8 );

    uint16_t prevSP = m_sp;
    
    if( BIT( r8, 7 ) )
    {
	uint8_t magnitude = ~r8;
	magnitude = magnitude + 1;
	m_sp -= magnitude;
    }
    else
    {	
	m_sp += r8;
    }
    
    carry = ( (prevSP & 0xFF) + r8 ) > 0xFF;
    halfCarry = ( (prevSP & 0xF) + (r8 & 0xF) ) > 0xF;	
    
    // Set flags
    this->setFlag( c_zero, false );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, halfCarry );
    this->setFlag( c_carry, carry );
    
    return 16;
}

uint16_t Z80::get2ByteRegValue( uint8_t high, uint8_t low )
{
    uint16_t value = high;
    value = value << 8;
    value += low;

    return value;
}

void Z80::split2ByteRegValue( uint8_t& high, uint8_t& low, uint16_t value)
{
    high = (uint8_t)(value >> 8);
    low = (uint8_t)(value & 0xFF);
}

void Z80::setFlag( uint8_t mask, bool value )
{
    if( value ) { m_flags |= mask; }
    else { m_flags &= (~mask); }
}

bool Z80::getFlag( uint8_t mask )
{
    bool flag = ( (m_flags & mask) != 0 );
    return flag;
}

void Z80::setHalfCarry( uint8_t prev, uint8_t delta, bool subtract, int offset )
{
    bool halfCarryFlag = false;
    
    if( subtract )
    {
	halfCarryFlag = ( ( ((int)prev & 0xF) -
			    ((int)delta & 0xF) -
			    offset ) < 0 );
    }
    else
    {
	halfCarryFlag = ( ( ((int)prev & 0xF) +
			    (delta & 0xF) + offset
			      ) & 0xF0 ) != 0;
    }

    this->setFlag( c_halfCarry, halfCarryFlag );
}

void Z80::setCarry( uint8_t prev, uint8_t delta, bool subtract, int offset )
{
    bool carryFlag = false;
    if( subtract )
    {
	carryFlag = ( ((int)prev - (int)delta - offset) < 0 );
    }
    else
    {
	carryFlag = ( ((int)prev + (int)delta + (int)offset)
		  & 0xF00 ) != 0;
    }
    
    this->setFlag( c_carry, carryFlag );
}

uint8_t Z80::fetchNextInstruction( void )
{
    uint8_t opcode;
    mp_bus->access( m_pc, opcode, READ );

    return opcode;
}

uint8_t Z80::rl( uint8_t& reg, bool hl )
{
    bool carry, nextCarry, nextZero;    
    uint8_t ticks = 8;

    carry = this->getFlag( c_carry );
    
    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;
	
	mp_bus->access( addr, val, READ );

	nextCarry = BIT( val, 7 );
	val = val << 1;
	if( carry ) { val |= 0x01; }
	nextZero = ( val == 0 );

	mp_bus->access( addr, val, WRITE );

	ticks = 16;
    }
    else
    {
	nextCarry = BIT( reg, 7 );
	reg = reg << 1;
	if( carry ) { reg |= 0x01; }
	nextZero = ( reg == 0 );
    }

    // Set flags
    this->setFlag( c_carry, nextCarry );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );
    this->setFlag( c_zero, nextZero ); 
    
    return ticks;
}

uint8_t Z80::rr( uint8_t& reg, bool hl )
{
    bool carry, nextCarry, nextZero;
    uint8_t ticks = 8;

    carry = this->getFlag( c_carry );
    
    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;

	mp_bus->access( addr, val, READ );

	nextCarry = BIT( val, 0 );
	val = val >> 1;
	if( carry ) { val |= 0x80; }
	nextZero = ( val == 0 );

	mp_bus->access( addr, val, WRITE );

	ticks = 16;
    }
    else
    {
	nextCarry = BIT( reg, 0 );
	reg = reg >> 1;
	if( carry ) { reg |= 0x80; }
	nextZero = ( reg == 0 );
    }

    // Set flags
    this->setFlag( c_carry, nextCarry );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );
    this->setFlag( c_zero, nextZero );
    
    
    return ticks;
}

uint8_t Z80::rlc( uint8_t& reg, bool hl )
{
    uint8_t ticks = 8;
    bool nextCarry, nextZero;
    
    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;
	
	mp_bus->access( addr, val, READ );

	nextCarry = BIT( val, 7 );
	val = val << 1;
	if( nextCarry ) { val |= 0x01; }
	nextZero = (val == 0);
	
	mp_bus->access( addr, val, WRITE );

	ticks = 16;
    }
    else
    {
	nextCarry = BIT( reg, 7 );
	reg = reg << 1;
	if( nextCarry ) { reg |= 0x01; }
	nextZero = (reg == 0);
    }
    
    // Set flags
    this->setFlag( c_zero, nextZero );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );    
    this->setFlag( c_carry, nextCarry );
    
    return ticks;
}

uint8_t Z80::rrc( uint8_t& reg, bool hl )
{
    uint8_t ticks = 8;
    bool nextCarry, nextZero;

    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;

	mp_bus->access( addr, val, READ );

	nextCarry = BIT( val, 0 );
	val = val >> 1;
	if( nextCarry ) { val |= 0x80; }
	nextZero = (val == 0);
	
	mp_bus->access( addr, val, WRITE );

	ticks = 16;
    }
    else
    {
	nextCarry = BIT( reg, 0 );
	reg = reg >> 1;
	if( nextCarry ) { reg |= 0x80; }
	nextZero = (reg == 0);
    }

    // Set flags
    this->setFlag( c_zero, nextZero );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );    
    this->setFlag( c_carry, nextCarry );   
    
    return ticks;
}

uint8_t Z80::sla( uint8_t& reg, bool hl )
{
    uint8_t ticks = 8;
    bool carry, zero;
    
    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;

	mp_bus->access( addr, val, READ );

	carry = BIT( val, 7 );
	val = val << 1;
	zero = ( val == 0x00 );

	mp_bus->access( addr, val, WRITE );

	ticks = 16;
    }
    else
    {
	carry = BIT( reg, 7 );
	reg = reg << 1;
	zero = ( reg == 0x00 );
    }

    // Set flags
    this->setFlag( c_zero, zero );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );
    this->setFlag( c_carry, carry );
    
    return ticks;
}

uint8_t Z80::sra( uint8_t& reg, bool hl )
{
    uint8_t ticks = 8;
    bool carry, zero;

    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;

	mp_bus->access( addr, val, READ );

	uint8_t prev = val & 0x80;
	carry = BIT( val, 0 );
	val = val >> 1;
	val |= prev;
	zero = ( val == 0x00 );

	mp_bus->access( addr, val, WRITE );

	ticks = 16;
    }
    else
    {
	uint8_t prev = reg & 0x80;

	carry = BIT( reg, 0 );
	reg = reg >> 1;
	reg |= prev;
	
	zero = ( reg == 0 );
    }

    // Set flags
    this->setFlag( c_zero, zero );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );
    this->setFlag( c_carry, carry );
    
    return ticks;
}

uint8_t Z80::swap( uint8_t& reg, bool hl )
{
    uint8_t ticks = 8;
    bool zero;

    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;

	mp_bus->access( addr, val, READ );

	uint8_t high, low;
	high = val >> 4;
	low = val & 0x0F;
	val = ( low << 4 ) | high;
	zero = ( val == 0x00 );
	
	mp_bus->access( addr, val, WRITE );
	
	ticks = 16;
    }
    else
    {
	uint8_t high, low;
	high = reg >> 4;
	low = reg & 0x0F;
	reg = ( low << 4 ) | high;
	zero = ( reg == 0x00 );
    }
    
    // Set flags
    this->setFlag( c_zero, zero );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );
    this->setFlag( c_carry, false );
    
    return ticks;
}

uint8_t Z80::srl( uint8_t& reg, bool hl )
{
    uint8_t ticks = 8;
    bool zero, carry;
    
    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;
	
	mp_bus->access( addr, val, READ );

	carry = BIT( val, 0 );
	val = val >> 1;
	zero = ( val == 0 );
	
	mp_bus->access( addr, val, WRITE );
	
	ticks = 16;
    }
    else
    {
	carry = BIT( reg, 0 );
	reg = reg >> 1;
	zero = ( reg == 0 );
    }

    // Set flags
    this->setFlag( c_zero, zero );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, false );
    this->setFlag( c_carry, carry );
    
    return ticks;
}


uint8_t Z80::bit( uint8_t& reg, uint8_t bit, bool hl )
{
    uint8_t ticks = 8;
    bool zero;
    
    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;
	
	mp_bus->access( addr, val, READ );

	zero = !BIT( val, bit );
	
	ticks = 12;
    }
    else
    {
	zero = !BIT( reg, bit );
    }

    // Set flags
    this->setFlag( c_zero, zero );
    this->setFlag( c_sub, false );
    this->setFlag( c_halfCarry, true );

    return ticks;
}

uint8_t Z80::res( uint8_t& reg, uint8_t bit, bool hl )
{
    uint8_t ticks = 8;

    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;
	
	mp_bus->access( addr, val, READ );
	val = RES( val, bit );
	mp_bus->access( addr, val, WRITE );

	ticks = 16;
    }
    else
    {
	reg = RES( reg, bit );
    }
    
    return ticks;
}

uint8_t Z80::set( uint8_t& reg, uint8_t bit, bool hl )
{
    uint8_t ticks = 8;

    if( hl )
    {
	uint16_t addr = this->get2ByteRegValue( m_h, m_l );
	uint8_t val;
	
	mp_bus->access( addr, val, READ );
	val = SET( val, bit );
	mp_bus->access( addr, val, WRITE );

	ticks = 16;
    }
    else
    {
	reg = SET( reg, bit );
    }
    
    return ticks;
}

uint8_t Z80::executeCBInstruction( uint8_t opcode )
{
    uint8_t ticks = 0;
    uint8_t high = opcode >> 4;
    uint8_t low = opcode & 0x0F;
    
    if( (opcode >= 0x00) && (opcode <= 0x07) )
    {
	// RLC
	switch( low )
	{
	case 0x00:
	    ticks = this->rlc( m_b );
	    break;
	case 0x01:
	    ticks = this->rlc( m_c );
	    break;
	case 0x02:
	    ticks = this->rlc( m_d );
	    break;
	case 0x03:
	    ticks = this->rlc( m_e );
	    break;
	case 0x04:
	    ticks = this->rlc( m_h );
	    break;
	case 0x05:
	    ticks = this->rlc( m_l );
	    break;
	case 0x06:
	    ticks = this->rlc( m_a, true );
	    break;
	case 0x07:
	default:
	    ticks = this->rlc( m_a );
	    break;
	}        
    }

    if( (opcode >= 0x08) && (opcode <= 0x0F) )
    {
	// RRC
	switch( low )
	{
	case 0x08:
	    ticks = this->rrc( m_b );
	    break;
	case 0x09:
	    ticks = this->rrc( m_c );
	    break;
	case 0x0A:
	    ticks = this->rrc( m_d );
	    break;
	case 0x0B:
	    ticks = this->rrc( m_e );
	    break;
	case 0x0C:
	    ticks = this->rrc( m_h );
	    break;
	case 0x0D:
	    ticks = this->rrc( m_l );
	    break;
	case 0x0E:
	    ticks = this->rrc( m_a, true );
	    break;
	case 0x0F:
	default:
	    ticks = this->rrc( m_a );
	    break;
	}     
    }

    if( (opcode >= 0x10) && (opcode <= 0x17) )
    {
	// RL
	switch( low )
	{
	case 0x00:
	    ticks = this->rl( m_b );
	    break;
	case 0x01:
	    ticks = this->rl( m_c );
	    break;
	case 0x02:
	    ticks = this->rl( m_d );
	    break;
	case 0x03:
	    ticks = this->rl( m_e );
	    break;
	case 0x04:
	    ticks = this->rl( m_h );
	    break;
	case 0x05:
	    ticks = this->rl( m_l );
	    break;
	case 0x06:
	    ticks = this->rl( m_a, true );
	    break;
	case 0x07:
	default:
	    ticks = this->rl( m_a );
	    break;
	}        
    }

    if( (opcode >= 0x18) && (opcode <= 0x1F) )
    {
	// RR
	switch( low )
	{
	case 0x08:
	    ticks = this->rr( m_b );
	    break;
	case 0x09:
	    ticks = this->rr( m_c );
	    break;
	case 0x0A:
	    ticks = this->rr( m_d );
	    break;
	case 0x0B:
	    ticks = this->rr( m_e );
	    break;
	case 0x0C:
	    ticks = this->rr( m_h );
	    break;
	case 0x0D:
	    ticks = this->rr( m_l );
	    break;
	case 0x0E:
	    ticks = this->rr( m_a, true );
	    break;
	case 0x0F:
	default:
	    ticks = this->rr( m_a );
	    break;
	}
    }

    if( (opcode >= 0x20) && (opcode <= 0x27) )
    {
	// SLA
	switch( low )
	{
	case 0x00:
	    ticks = this->sla( m_b );
	    break;
	case 0x01:
	    ticks = this->sla( m_c );
	    break;
	case 0x02:
	    ticks = this->sla( m_d );
	    break;
	case 0x03:
	    ticks = this->sla( m_e );
	    break;
	case 0x04:
	    ticks = this->sla( m_h );
	    break;
	case 0x05:
	    ticks = this->sla( m_l );
	    break;
	case 0x06:
	    ticks = this->sla( m_a, true );
	    break;
	case 0x07:
	default:
	    ticks = this->sla( m_a );
	    break;
	}
    }

    if( (opcode >= 0x28) && (opcode <= 0x2F) )
    {
	// SRA
	switch( low )
	{
	case 0x08:
	    ticks = this->sra( m_b );
	    break;
	case 0x09:
	    ticks = this->sra( m_c );
	    break;
	case 0x0A:
	    ticks = this->sra( m_d );
	    break;
	case 0x0B:
	    ticks = this->sra( m_e );
	    break;
	case 0x0C:
	    ticks = this->sra( m_h );
	    break;
	case 0x0D:
	    ticks = this->sra( m_l );
	    break;
	case 0x0E:
	    ticks = this->sra( m_a, true );
	    break;
	case 0x0F:
	default:
	    ticks = this->sra( m_a );
	    break;
	}
    }

    if( (opcode >= 0x30) && (opcode <= 0x37) )
    {
	// SWAP
	switch( low )
	{
	case 0x00:
	    ticks = this->swap( m_b );
	    break;
	case 0x01:
	    ticks = this->swap( m_c );
	    break;
	case 0x02:
	    ticks = this->swap( m_d );
	    break;
	case 0x03:
	    ticks = this->swap( m_e );
	    break;
	case 0x04:
	    ticks = this->swap( m_h );
	    break;
	case 0x05:
	    ticks = this->swap( m_l );
	    break;
	case 0x06:
	    ticks = this->swap( m_a, true );
	    break;
	case 0x07:
	default:
	    ticks = this->swap( m_a );
	    break;
	}
    }

    if( (opcode >= 0x38) && (opcode <= 0x3F) )
    {
	// SRL
	switch( low )
	{
	case 0x08:
	    ticks = this->srl( m_b );
	    break;
	case 0x09:
	    ticks = this->srl( m_c );
	    break;
	case 0x0A:
	    ticks = this->srl( m_d );
	    break;
	case 0x0B:
	    ticks = this->srl( m_e );
	    break;
	case 0x0C:
	    ticks = this->srl( m_h );
	    break;
	case 0x0D:
	    ticks = this->srl( m_l );
	    break;
	case 0x0E:
	    ticks = this->srl( m_a, true );
	    break;
	case 0x0F:
	default:
	    ticks = this->srl( m_a );
	    break;
	}
    }

    if( (opcode >= 0x40) && (opcode <= 0x7F) )
    {
	// BIT n,-
	uint8_t val = (2 * (high - 0x04)) + (low / 0x08);
	
	switch( low )
	{
	case 0x00:
	case 0x08:
	    ticks = this->bit( m_b, val );
	    break;
	case 0x01:
	case 0x09:
	    ticks = this->bit( m_c, val );
	    break;
	case 0x02:
	case 0x0A:
	    ticks = this->bit( m_d, val );
	    break;
	case 0x03:
	case 0x0B:
	    ticks = this->bit( m_e, val );
	    break;
	case 0x04:
	case 0x0C:
	    ticks = this->bit( m_h, val );
	    break;
	case 0x05:
	case 0x0D:
	    ticks = this->bit( m_l, val );
	    break;
	case 0x06:
	case 0x0E:
	    ticks = this->bit( m_a, val, true );
	    break;
	case 0x07:
	case 0x0F:
	default:
	    ticks = this->bit( m_a, val );
	    break;
	}
    }

    if( (opcode >= 0x80) && (opcode <= 0xBF) )
    {
	// RES n,-
	uint8_t val = (2 * (high - 0x08)) + (low / 0x08);
	
	switch( low )
	{
	case 0x00:
	case 0x08:
	    ticks = this->res( m_b, val );
	    break;
	case 0x01:
	case 0x09:
	    ticks = this->res( m_c, val );
	    break;
	case 0x02:
	case 0x0A:
	    ticks = this->res( m_d, val );
	    break;
	case 0x03:
	case 0x0B:
	    ticks = this->res( m_e, val );
	    break;
	case 0x04:
	case 0x0C:
	    ticks = this->res( m_h, val );
	    break;
	case 0x05:
	case 0x0D:
	    ticks = this->res( m_l, val );
	    break;
	case 0x06:
	case 0x0E:
	    ticks = this->res( m_a, val, true );
	    break;
	case 0x07:
	case 0x0F:
	default:
	    ticks = this->res( m_a, val );
	    break;
	}
    }

    if( (opcode >= 0xC0) && (opcode <= 0xFF) )
    {
	// SET n,-
	uint8_t val = (2 * (high - 0x0C)) + (low / 0x08);
	
	switch( low )
	{
	case 0x00:
	case 0x08:
	    ticks = this->set( m_b, val );
	    break;
	case 0x01:
	case 0x09:
	    ticks = this->set( m_c, val );
	    break;
	case 0x02:
	case 0x0A:
	    ticks = this->set( m_d, val );
	    break;
	case 0x03:
	case 0x0B:
	    ticks = this->set( m_e, val );
	    break;
	case 0x04:
	case 0x0C:
	    ticks = this->set( m_h, val );
	    break;
	case 0x05:
	case 0x0D:
	    ticks = this->set( m_l, val );
	    break;
	case 0x06:
	case 0x0E:
	    ticks = this->set( m_a, val, true );
	    break;
	case 0x07:
	case 0x0F:
	default:
	    ticks = this->set( m_a, val );
	    break;
	}
    }
    
    return ticks;
}

