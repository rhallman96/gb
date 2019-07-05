#pragma once

#include <unordered_map>
#include <string>

using std::unordered_map;
using std::string;

/**
 * @author Rick Hallman
 * Maps keyboard input to scancode values and writes to
 * a config file.
 */
class Config
{
public:

    /**
     * Constructor.
     * @param path config file's path
     */
    Config( string path );
    ~Config( void );

    /**
     * Adds a button mapping to the config file.
     * @param button the button being mapped
     * @param value the keyboard value
     */
    void addMapping( string button, int value  );
    
    /**
     * Attempts to write the key mapping to a config file.
     */
    bool write( void );
    
private:
    
    /**
     * The button -> value map.
     */
    unordered_map<string, int> m_buttonMap;

    // the config file's absolute path
    string m_path;
    
};
