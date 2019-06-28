#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <string>

using std::string;

/**
 * @author Rick Hallman
 * This is the launcher's toolbar. Event handlers should
 * be added from the containing frame.
 */
class ToolBar : public wxToolBar
{
public:

    /**
     * Constructor.
     * @param parent the parent window
     * @param directory the program's base directory
     */
    ToolBar( wxWindow* parent, string directory );
    virtual ~ToolBar( void );
    
    enum ControlIDs
    {
	ID = 2,
	ID_PLAY = 3,
	ID_CONTROLS = 4
    };

private:

    // relative directory for icons
    static const string c_iconDir;
	
    // icon file names
    static const string c_playIcon;
    static const string c_controlsIcon;

    // absolute icon directory
    string m_iconDir;
};
