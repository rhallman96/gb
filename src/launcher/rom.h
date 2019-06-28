#pragma once

#include <string>
using std::string;

/**
 * @author Rick Hallman
 * This class represents a playable ROM file.
 */
class Rom
{
public:
    
    // ROM and save file extensions
    static const string c_romExtension;
    static const string c_saveExtension;
    
    /**
     * Constructor.
     * @param name the game's name
     * @param romPath the rom's absolute path
     * @param savePath the save's absolute path
     */
    Rom( string name, string romPath, string savePath );

    /**
     * Gets the rom file's absolute path.
     * @return the rom file's absolute path
     */
    string getRomPath( void );

    /**
     * Gets the save file's absolute path.
     * @return the save file's absolute path
     */
    string getSavePath( void );
    
private:

    // the game's name
    string m_name;
    
    // the ROM and its save path
    string m_romPath, m_savePath;
};
