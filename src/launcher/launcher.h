#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "rom.h"
#include "romlist.h"
#include "toolbar.h"

#include <string>

using std::string;

/**
 * @author Rick Hallman
 * This class represents the launcher's window frame.
 */
class LaunchFrame : public wxFrame
{
public:
    /**
     * Constructor.
     * @param directory the base directory
     */
    LaunchFrame( string directory );
    ~LaunchFrame( void );
    
private:

    // the frame's title
    static const string c_title;
    
    /**
     * Event handler for exiting.
     * @param event event args
     */
    void OnExit( wxCommandEvent& event );

    /**
     * Event handler for ROM double click.
     * @param event event args
     */
    void OnRomDoubleClick( wxCommandEvent& event );

    /**
     * Event handler for the toolbar's play button.
     * @param event event args
     */
    void OnPlayClicked( wxCommandEvent& event );

    /**
     * Event handler for the toolbar's controls button.
     * @param event event args
     */    
    void OnControlsClicked( wxCommandEvent& event );
    
    wxDECLARE_EVENT_TABLE();
    
    /**
     * Launches a given ROM.
     */
    void launch( Rom* rom );

    /**
     * Launches the control configuration dialog.
     */
    void setControls( void );

    // the default width and height
    static const int c_width = 400;
    static const int c_height = 300;
    
    // layout elements
    ToolBar* mp_toolBar;
    RomList* mp_romList;
    
    // the emulator's path
    string m_gbPath;

    // the key reader's path
    string m_keyReaderPath;

    // the config file's path
    string m_configPath;
};

/**
 * @author Rick Hallman
 * This class represents the launcher as a wxWidgets app.
 */
class Launcher : public wxApp
{
public:
    Launcher( void );
    ~Launcher( void );
    virtual bool OnInit( void );

    // the emulator's name
    static const string c_gb;

    // the key reader's name
    static const string c_keyReader;

    // the keyboard config file's name
    static const string c_config;
    
private:
    
    LaunchFrame* mp_frame;

    // the base directory
    string m_directory;    
};

