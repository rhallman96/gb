#include "window.h"
#include <iostream>

Window::Window( GB* gb )
    : mp_gb( gb )
{
    SDL_Init( SDL_INIT_VIDEO );
    
    SDL_CreateWindowAndRenderer( c_width, c_height,
				 SDL_WINDOW_RESIZABLE,
				 &mp_window,
				 &mp_renderer );
    SDL_RenderSetLogicalSize( mp_renderer, c_logWidth, c_logHeight );

    m_prevTicks = SDL_GetTicks();
}

void Window::run( void )
{   
    SDL_Event e;
    bool done = false;

    while( done == false )
    {
	// Update the emulator
	if( mp_gb->update() != 0 ) { done = true; }
	
	// Draw the screen if LCD is ready
	if( mp_gb->getLCD()->readyToDraw() )
	{
	    // Poll window events
	    while( SDL_PollEvent( &e ) )
	    {
		if( e.type == SDL_QUIT ) { done = true; }
	    }

	    // Render to the screen
	    this->render();

	    // Update the joypad based on keyboard input
	    mp_gb->getJoyPad()->readKeyboard();

	    // Delay accordingly
	    Uint32 delta = SDL_GetTicks() - m_prevTicks;
	    m_prevTicks += delta;
	    if( delta < c_delay ) { SDL_Delay( c_delay - delta ); }
	}
    }

    mp_gb->close();
}

void Window::render( void )
{
    SDL_SetRenderDrawColor( mp_renderer, 0, 0, 0, 0 );
    SDL_RenderClear( mp_renderer ); 
    
    int color;
    uint8_t r, g, b, a;

    for( int y = 0; y < 144; y++ )
    {
	for( int x = 0; x < 160; x++ )
	{
	    color = mp_gb->getLCD()->getPixel( x, y );
	    r = color & 0xFF;
	    g = (color >> 8) & 0xFF;
	    b = (color >> 16) & 0xFF;
	    a = (color >> 24) & 0xFF;
	    
	    SDL_SetRenderDrawColor( mp_renderer, r, g, b, a );
	    SDL_RenderDrawPoint( mp_renderer, x, y );
	}
    }
        
    SDL_RenderPresent( mp_renderer );
}

Window::~Window( void )
{
    SDL_DestroyWindow( mp_window ); mp_window = NULL;
    SDL_DestroyRenderer( mp_renderer ); mp_renderer = NULL;
    SDL_Quit();
}

