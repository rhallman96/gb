#include "lcd.h"

#include <iostream>

LCD::LCD( Z80* z80, Bus* bus )
    : mp_z80( z80 ),
      mp_bus( bus ),
      m_counter( 0x0000 ),
      m_readyToDraw( false )
{
}

LCD::~LCD( void )
{
}

int LCD::getPixel( int x, int y )
{
    if( (x < 0) || (x >= 160) || (y < 0) || (y >= 144) )
    {
	return 0;
    }

    return m_pixels[y][x];
}

void LCD::update( uint8_t ticks )
{
    // set the LCD's status
    this->setStatus();

    // update LCD counter based on ticks
    m_counter += ticks;

    if( m_counter >= c_cycle )
    {
	m_counter -= c_cycle;

	uint8_t scanline;
	mp_bus->defaultAccess( 0xFF44, scanline, READ );

	scanline++;
	if( scanline > 153 ) { scanline = 0; }

        // Handle coincidence register
	uint8_t lyc, stat;
	mp_bus->defaultAccess( 0xFF45, lyc, READ );
	mp_bus->defaultAccess( 0xFF41, stat, READ ); 
	if( lyc == scanline )
	{
	    stat = SET( stat, 2 );
	    if( BIT( stat, 6 ) == true )
	    {
		mp_z80->triggerInterrupt( Z80::c_LCDStat );
	    }
	}
	else
	{
	    stat = RES( stat, 2 );
	}
	mp_bus->defaultAccess( 0xFF41, stat, WRITE );
	
	if( scanline == 144 )
	{
	    m_readyToDraw = true;
	    
	    // vblank interrupt
	    mp_z80->triggerInterrupt( Z80::c_vBlank );
	}

	// write updated scanline to address
	mp_bus->defaultAccess( 0xFF44, scanline, WRITE );
    }
}

bool LCD::isEnabled( void )
{
    uint8_t lcdc;
    mp_bus->defaultAccess( 0xFF40, lcdc, READ );
    
    bool enabled = BIT(lcdc, 7);
    return enabled;
}

bool LCD::readyToDraw( void )
{
    if( m_readyToDraw )
    {
	m_readyToDraw = false;
	return true;
    }
    
    return false;
}

void LCD::drawPixel( int x, int y, int color )
{
    if( (x < 0) || (x >= 160) || (y < 0) || (y >= 144) )
    {
	return;
    }

    m_pixels[y][x] = color;
}


void LCD::drawScanline( void )
{
    uint8_t scanline;
    mp_bus->defaultAccess( 0xFF44, scanline, READ );
    
    if( scanline >= 144 ) { return; }
    
    uint8_t lcdc;
    mp_bus->defaultAccess( 0xFF40, lcdc, READ );

    if( BIT(lcdc, 0) ) { this->drawTiles( lcdc, scanline ); }
    if( BIT(lcdc, 1) ) { this->drawSprites( lcdc, scanline ); }
}

void LCD::drawTiles( uint8_t lcdc, uint8_t scanline )
{
    // drawing registers
    uint8_t scy, scx, palette, wy, wx;
    
    mp_bus->defaultAccess( 0xFF42, scy, READ );
    mp_bus->defaultAccess( 0xFF43, scx, READ );
    mp_bus->defaultAccess( 0xFF47, palette, READ );
    mp_bus->defaultAccess( 0xFF4A, wy, READ );
    mp_bus->defaultAccess( 0xFF4B, wx, READ );

    // set tile data address
    uint16_t tileData;
    if( BIT( lcdc, 4 ) ) { tileData = 0x8000; }
    else { tileData = 0x9000; }

    // using window or not
    bool windowEnabled = BIT( lcdc, 5 );
    
    // set tile map display
    uint16_t windowMap, bgMap;
    if( BIT( lcdc, 6 ) ) { windowMap = 0x9C00; }
    else { windowMap = 0x9800; }
    if( BIT( lcdc, 3 ) ) { bgMap = 0x9C00; }
    else { bgMap = 0x9800; }

    // get palette colors
    int colors[4] = { 0 };
    colors[3] = this->getColor( BIT( palette, 7 ),
				BIT( palette, 6 ) );    
    colors[2] = this->getColor( BIT( palette, 5 ),
				BIT( palette, 4 ) );
    colors[1] = this->getColor( BIT( palette, 3 ),
				BIT( palette, 2 ) );
    colors[0] = this->getColor( BIT( palette, 1 ),
				BIT( palette, 0 ) );

    
    // loop across pixels in scanline
    for( int x = 0; x < 160; x++ )
    {
	// logical x and y positions
	int yPos = scanline + scy;
	int xPos = x + scx;

	// calculate map address
	uint16_t map = bgMap;
	
	if( windowEnabled )
	{
	    int windowX = (int)wx - 7;
	    if( (x >= windowX) && (scanline >= wy) )
	    {
		xPos = x - windowX;
		yPos = scanline - wy;
		map = windowMap;
	    }
	}
	
	// calculate tile address
	int tileRow = (yPos / 8) % 32;
	int tileCol = (xPos / 8) % 32;
	int tileIndex = (tileRow * 32) + tileCol;
        uint8_t tileOffset;

	mp_bus->defaultAccess( map + tileIndex, tileOffset, READ );

	uint16_t tileAddr = 0;
	if( tileData == 0x8000 )
	{
	    tileAddr = tileData + (tileOffset * 16);
	}
	else
	{
	    if( BIT( tileOffset, 7 ) == false )
	    {
		tileAddr = tileData + (tileOffset * 16);
	    }
	    else
	    {
		uint8_t offset = ~tileOffset;
		offset = offset + 1;
		tileAddr = tileData - (offset * 16);
	    }
	}
	
	tileAddr += ( 2 * (yPos % 8) );

	// get tile color
	uint8_t line1, line2;
	mp_bus->defaultAccess( tileAddr, line1, READ );
	mp_bus->defaultAccess( tileAddr + 1, line2, READ );
	
	bool low = BIT( line1, (7 - (xPos % 8)) );
	bool high = BIT( line2, (7 - (xPos % 8)) );

	uint8_t colorIndex = ( high ? 2 : 0 ) + ( low ? 1 : 0 );
	this->drawPixel( x, scanline, colors[colorIndex] );
    }
}

void LCD::drawSprites( uint8_t lcdc, uint8_t scanline )
{
    // calculate sprite size
    int size = 8;
    if( BIT( lcdc, 2 ) ) { size = 16; }

    // get background palette color 0
    uint8_t bgPalette;
    int bgColor0;
    mp_bus->defaultAccess( 0xFF47, bgPalette, READ );
    bgColor0 = this->getColor( BIT( bgPalette, 1 ),
			       BIT( bgPalette, 0 ) );

    // keeps track of whether or not pixels were set
    bool pixelsSet[160] = {false};
    
    // loop over sprites
    int spriteCount = 0;

    for( uint16_t addr = 0xFE00; addr < 0xFEA0; addr += 4 )
    {
	if( spriteCount == 10 ) { break; }
	
	bool valid = this->drawSprite( addr, scanline, size, bgColor0, pixelsSet );
	if( valid ) { spriteCount++; }
    }
}

bool LCD::drawSprite( uint16_t addr, uint8_t scanline, int size, int bgColor0, bool pixelsSet[] )
{
    // sprite information
    uint8_t x, y, tileNum, attributes;
    mp_bus->defaultAccess( addr, y, READ );
    mp_bus->defaultAccess( addr + 1, x, READ );
    mp_bus->defaultAccess( addr + 2, tileNum, READ );
    mp_bus->defaultAccess( addr + 3, attributes, READ );
    int yPos = y - 16;
    int xPos = x - 8;
	
    if( (scanline < yPos) || (scanline >= (yPos + size)) )
    {
	return false;
    }
	
    // attributes
    bool xFlip = BIT( attributes, 5 );
    bool yFlip = BIT( attributes, 6 );
    bool bgPriority = BIT( attributes, 7 );
	
    // color palette
    uint16_t paletteAddr;
    uint8_t palette;
    if( BIT( attributes, 4 ) ) { paletteAddr = 0xFF49; }
    else { paletteAddr = 0xFF48; }
    mp_bus->defaultAccess( paletteAddr, palette, READ );
    int colors[4] = { 0 };
    colors[3] = this->getColor( BIT( palette, 7 ),
				BIT( palette, 6 ) );    
    colors[2] = this->getColor( BIT( palette, 5 ),
				BIT( palette, 4 ) );
    colors[1] = this->getColor( BIT( palette, 3 ),
				BIT( palette, 2 ) );
    colors[0] = this->getColor( BIT( palette, 1 ),
				BIT( palette, 0 ) );
		
    // calculate line number
    int line = scanline - yPos;
    if( yFlip ) { line = size - line - 1; }
	
    // sprite data
    uint16_t spriteAddr = 0x8000 + (tileNum << 4) + (line * 2);
    if( size == 16 ) { spriteAddr -= (spriteAddr % 2); }
    uint8_t line1, line2;
    mp_bus->defaultAccess( spriteAddr, line1, READ );
    mp_bus->defaultAccess( spriteAddr + 1, line2, READ );

    // whether or not the current pixel has priority
    bool hasPriority = !pixelsSet[xPos];
    
    for( int pixel = 0; pixel < 8; pixel++ )
    {
	if( xPos + pixel >= 160 ) { break; }
	else if( (xPos + pixel < 0) || (xPos + pixel >= 160) ) { continue; };

	// check sprite priority
	if( (hasPriority == false) &&
	    (pixelsSet[xPos + pixel] == false) )
	{
	    hasPriority = true;
	}

	if( hasPriority == false ) { continue; }
	
	// if bg priority set to 1, only draw over color 0
	int bgColor = this->getPixel( xPos + pixel, scanline );
	if( bgPriority && ( bgColor != bgColor0 ) ) { continue; }
	    
	// get color
	uint8_t bit = pixel;
	if( xFlip == false ) { bit = 7 - pixel; }
	bool low = BIT( line1, bit );
	bool high = BIT( line2, bit );
	uint8_t colorIndex = ( high ? 2 : 0 ) + ( low ? 1 : 0 );

	if( colorIndex == 0 ) { continue; }
	int color = colors[colorIndex];
	    
	this->drawPixel( xPos + pixel, scanline, color );

	pixelsSet[xPos + pixel] = true;
    }

    return true;
}

int LCD::getColor( bool high, bool low )
{
    uint8_t val = 0;
    val |= ( high ? 0x10 : 0 );
    val |= ( low ? 0x01 : 0 );

    switch( val )
    {
    case 0x00:
	return c_white;
    case 0x01:
	return c_lightGray;
    case 0x10:
	return c_darkGray;
    case 0x11:
    default:
	return c_black;
    }
}

void LCD::setStatus( void )
{
    // status register
    uint8_t stat;
    mp_bus->defaultAccess( 0xFF41, stat, READ );
    
    if( this->isEnabled() == false )
    {
	// MODE 1
	this->setMode( stat, 0x01 );

	// reset coincidence flag
	stat = RES( stat, 2 );
	mp_bus->defaultAccess( 0xFF41, stat, WRITE );

	// reset LCD y coordinate
	uint8_t s = 0;
	mp_bus->defaultAccess( 0xFF44, s, WRITE );
	
	m_counter = 0;
	
	return;
    }

    uint8_t scanline;
    mp_bus->defaultAccess( 0xFF44, scanline, READ );

    // use for triggering interrupts
    uint8_t prevMode = stat & 0x03;
    bool interrupt = false;
    
    if( scanline >= 144 )
    {
	// MODE 1
	this->setMode( stat, 0x01 );
	interrupt = BIT( stat, 4 );
    }
    else
    {
	if( m_counter < c_mode2 )
	{  
	    // MODE 2
	    this->setMode( stat, 0x02 );
	    interrupt = BIT( stat, 5 );
	}
	else if( m_counter < c_mode3 )
	{
	    // MODE 3
	    this->setMode( stat, 0x03 );
	}
	else
	{
	    if( prevMode == 3 )
	    {
		this->drawScanline();
	    }
	    
	    // MODE 0
	    this->setMode( stat, 0x00 );
	    interrupt = BIT( stat, 3 );
	}
    }    
    
    // Handle interrupts
    uint8_t mode = stat & 0x03;
    if( interrupt && ( mode != prevMode ) )
    {
	mp_z80->triggerInterrupt( Z80::c_LCDStat );
    }

    mp_bus->defaultAccess( 0xFF41, stat, WRITE );
}

void LCD::setMode( uint8_t& stat, uint8_t mode )
{
    mode = mode & 0x03;

    stat = RES( stat, 0 );
    stat = RES( stat, 1 );
    stat = stat | mode;
}
