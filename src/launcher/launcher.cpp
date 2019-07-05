#include "launcher.h"

#include <libgen.h>

// wxWidgets event macros
wxBEGIN_EVENT_TABLE(LaunchFrame, wxFrame)
    EVT_MENU(wxID_EXIT, LaunchFrame::OnExit)
    EVT_LISTBOX_DCLICK( RomList::ID, LaunchFrame::OnRomDoubleClick )
    EVT_TOOL( ToolBar::ID_PLAY, LaunchFrame::OnPlayClicked )
    EVT_TOOL( ToolBar::ID_CONTROLS, LaunchFrame::OnControlsClicked )
wxEND_EVENT_TABLE()

// the window's title
const string LaunchFrame::c_title = "GBEmu";

LaunchFrame::LaunchFrame( string directory ) :
    wxFrame( NULL, wxID_ANY, c_title, wxDefaultPosition, wxSize( c_width, c_height ) ),
    m_gbPath( directory + Launcher::c_gb ),
    m_keyReaderPath( directory + Launcher::c_keyReader ),
    m_configPath( directory + Launcher::c_config )
{
    mp_romList = new RomList( this, directory );
    mp_toolBar = new ToolBar( this, directory );
    
    this->SetToolBar( mp_toolBar );
    mp_toolBar->Realize();
}

LaunchFrame::~LaunchFrame( void )
{
}

void LaunchFrame::OnExit( wxCommandEvent& event )
{
    Close( true );
}

void LaunchFrame::OnRomDoubleClick( wxCommandEvent& event )
{
    Rom* rom = mp_romList->getSelection();
    this->launch( rom );
}

void LaunchFrame::OnPlayClicked( wxCommandEvent& event )
{
    Rom* rom = mp_romList->getSelection();
    this->launch( rom );
}

void LaunchFrame::OnControlsClicked( wxCommandEvent& event )
{
    this->setControls();
}

void LaunchFrame::launch( Rom* rom )
{
    if( rom == NULL ) { return; }
    
    pid_t pid = fork();
    string romPath = rom->getRomPath();
    string savePath = rom->getSavePath();
    
    if( pid == 0 )
    {
	const char* path = m_gbPath.c_str();
	
	// child process
	char* args[4];
	args[0] = (char*) path;
	args[1] = (char*) romPath.c_str();
	args[2] = (char*) savePath.c_str();
	args[3] = NULL;

	execv( path, args );
    }
    else if( pid < 0 )
    {
	// fork error
	exit( 1 );
    }
}

void LaunchFrame::setControls( void )
{    
    pid_t pid = fork();
    
    if( pid == 0 )
    {
	const char* path = m_keyReaderPath.c_str();
	const char* config = m_configPath.c_str();
	
	// child process
	char* args[3];
	args[0] = (char*) path;
	args[1] = (char*) config;
	args[2] = NULL;
	
	execv( path, args );
    }
    else if( pid < 0 )
    {
	// fork error
	exit( 1 );
    }
}

// set launcher as the app
wxIMPLEMENT_APP(Launcher);

// gb executable name
const string Launcher::c_gb = "gb";

// key reader name
const string Launcher::c_keyReader = "keyreader";

// config file name
const string Launcher::c_config = "keys.config";

Launcher::Launcher( void ) :
    mp_frame( NULL )
{
}

Launcher::~Launcher( void )
{
}

bool Launcher::OnInit( void )
{
    char* buffer = NULL;
    buffer = realpath( argv[0], NULL );
    m_directory = string( dirname(buffer) ) + "/";
    free( buffer );
    
    // this frame will be deleted after Close is called
    mp_frame = new LaunchFrame( m_directory );
    mp_frame->Show( true );
    
    return true;
}
