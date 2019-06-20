#pragma once

#include "bus.h"
#include "devices.h"
#include "z80.h"

#include <SDL2/SDL.h>

/**
 * @author Rick Hallman
 * This class represents the LCD at its most abstract level.
 */
class LCD
{
public:

    static const uint32_t c_black = 0xFF000000;
    static const uint32_t c_darkGray = 0xFF555555;
    static const uint32_t c_lightGray = 0xFFAAAAAA;
    static const uint32_t c_white = 0xFFFFFFFF;
    
    /**
     * Constructor.
     * @param z80 the cpu
     * @param bus the system's bus
     */
    LCD( Z80* z80, Bus* bus );
    ~LCD( void );

    /**
     * Gets the color of a pixel on the LCD.
     * @param x the pixel's x coordinate
     * @param y the pixel's y coordinate
     * @return the pixel's color
     */
    int getPixel( int x, int y );
    
    /**
     * Update the LCD.
     * @param ticks the number of CPU ticks taken by the last instruction
     */
    void update( uint8_t ticks );

    /**
     * Whether or not the LCD is enabled.
     * @return true if enabled, false otherwise
     */
    bool isEnabled( void );

    /**
     * Whether or not the next frame is ready to be drawn.
     * Resets the draw flag immediately after true is returned.
     * @return true if yes, false otherwise
     */
    bool readyToDraw( void );
    
private:

    Z80* mp_z80;
    Bus* mp_bus;
    uint32_t m_counter;

    int m_pixels[144][160];
    
    bool m_readyToDraw;
    
    static const uint32_t c_cycle = 456;
    static const uint32_t c_mode2 = 80;
    static const uint32_t c_mode3 = 172 + c_mode2;

    /**
     * Draws a pixel to the renderer.
     * @param x the pixel's x coordinate
     * @param y the pixel's y coordinate
     * @param color the pixel's color (32 bit)
     */
    void drawPixel( int x, int y, int color );
    
    /**
     * Draw the current scanline on the LCD.
     */
    void drawScanline( void );

    /**
     * Draw tiles to a scanline.
     * @param lcdc the lcdc register value
     * @param scanline the scanline register value
     */
    void drawTiles( uint8_t lcdc, uint8_t scanline );

    /**
     * Draw sprites to a scanline.
     * @param lcdc the lcdc register value
     * @param scanline the scanline register value
     */
    void drawSprites( uint8_t lcdc, uint8_t scanline );

    /**
     * Draws an individual sprite.
     * @param addr the sprite's address
     * @param scanline the scanline y position
     * @param size the sprite's size (8 or 16)
     * @param bgColor0 the background palette's color assigned to 0
     * @param pixelsSet keeps track of which pixels have been drawn
     * @return true if drawing succeeded, false otherwise
     */
    bool drawSprite( uint16_t addr, uint8_t scanline, int size, int bgColor0, bool pixelsSet[] );
    
    /**
     * Gets the palette color
     * @param high the high color bit
     * @param the low color bit
     * @return the color associated with this bit
     */
    int getColor( bool high, bool low );
    
    /**
     * Sets the LCD's status.
     * If the status changes from 3 to 0 (i.e, the
     * LCD has entered H-Blank), this method will
     * draw the next scanline.
     */
    void setStatus( void );
    
    /**
     * Sets the LCD status register's mode. Does NOT write back to the bus.
     * @param stat a reference to the status value
     * @param mode the mode value
     */
     void setMode( uint8_t& stat, uint8_t mode );
};
