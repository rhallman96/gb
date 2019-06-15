#pragma once

#include "gb/gb.h"
#include "gb/lcd.h"
#include "gb/joypad.h"

#include <SDL2/SDL.h>

/**
 * @author Rick Hallman
 * This class is a wrapper for an SDL window. Used for updating and rendering
 * emulator content and handling input at a system level.
 */
class Window
{
public:

    /**
     * Constructor.
     * @param gb a reference to the logical component of the emulator
     */
    Window( GB* gb );
    ~Window( void );

    /**
     * Runs the emulator.
     */
    void run( void );
    
private:

    /**
     * Renders the contents of the window.
     */
    void render( void );
    
    // the window's initial and logical sizes
    static const int c_logWidth = 160;
    static const int c_logHeight = 144;
    static const int c_width = c_logWidth * 4;
    static const int c_height = c_logHeight * 4;

    // the logical portion of the emulator
    GB* mp_gb;

    // delay variables
    static const Uint32 c_delay = (1020 / 60);
    Uint32 m_prevTicks;
    
    // SDL objects
    SDL_Window* mp_window;
    SDL_Renderer* mp_renderer;
};
