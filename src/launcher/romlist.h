#pragma once

#include "rom.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/listctrl.h>

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

/**
 * @author Rick Ha llman
 * This control contains the list of games used
 * by the launcher.
 */
class RomList : public wxListBox
{
public:

    /**
     * Constructor.
     * @param parent the parent window
     * @param directory the program's base directory
     */
    RomList( wxWindow* parent, string directory );
    virtual ~RomList( void );

    /**
     * Gets the currently selected ROM.
     * @return the current selection, or NULL 
     */
    Rom* getSelection( void );

    enum ControlIDs
    {
	ID = 1
    };
    
private:

    // relative directory paths
    static const string c_romDir;
    static const string c_saveDir;

    // the ROM and save directories
    string m_romDir, m_saveDir;
    
    /**
     * Populate the contents of the game list.
     */ 
    void populate( void );

    // mapping of name => Rom object
    unordered_map<string, Rom*> m_romMap;

};
