#include "toolbar.h"

// icon file names
const string ToolBar::c_iconDir = "icon/";
const string ToolBar::c_playIcon = "play.png";
const string ToolBar::c_controlsIcon = "controller.png";

ToolBar::ToolBar( wxWindow* parent, string directory ) :
    wxToolBar( parent, ID ),
    m_iconDir( directory + c_iconDir )
{
    wxImage::AddHandler(new wxPNGHandler);

    string playPath = m_iconDir + c_playIcon;
    wxBitmap play(playPath.c_str(), wxBITMAP_TYPE_PNG);

    string controlsPath = m_iconDir + c_controlsIcon;
    wxBitmap controls( controlsPath.c_str(), wxBITMAP_TYPE_PNG);

    this->AddTool( ID_PLAY, wxT("Play"), play );
    this->AddTool( ID_CONTROLS, wxT("Controls"), controls );
}

ToolBar::~ToolBar( void )
{
}
