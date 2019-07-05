#pragma once

#include "config.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>

using std::string;

/**
 * @author Rick Hallman
 * Grabs keyboard input for the emulator and saves it
 * to a config file.
 */
class KeyReader
{
public:
    
    /**
     * Constructor.
     * @param path the program's base directory
     */
    KeyReader( string path );
    ~KeyReader( void );

    /**
     * Runs the key reader window.
     */
    void run( void );
    
private:

    // the window's size
    static const int c_width = 203;
    static const int c_height = 145;
    
    /**
     * The key reader's input state.
     */
    static const int STATE_A = 0;
    static const int STATE_B = 1;
    static const int STATE_START = 2;
    static const int STATE_SELECT = 3;
    static const int STATE_LEFT = 4;
    static const int STATE_RIGHT = 5;
    static const int STATE_UP = 6;
    static const int STATE_DOWN = 7;
    static const int STATE_DONE = 8;
    int m_state;

    // the icon directory
    static const string c_iconDir;
    
    /**
     * Loads the images for each state.
     * @param path the icon folder's path
     */
    void loadStateImages( string path );

    /**
     * Loads a given SDL surface from a given path.
     * @param path the image's path
     */
    SDL_Texture* loadImage( string path );
    
    /**
     * Handles a key being pressed.
     * @param e the key press event
     */
    void handleKeyPress( SDL_KeyboardEvent& e );    
    
    /**
     * Renders the contents of the window.
     */
    void render( void );

    /**
     * Gets the current button value based on the value of m_state.
     * @returns a string representing the current button
     */
    string getButtonValue( void );
    
    // the config file
    Config* mp_config;

    // images for each distinct state
    SDL_Texture* m_stateImages[ STATE_DONE ];
    
    // SDL objects
    SDL_Window* mp_window;
    SDL_Renderer* mp_renderer;
};
