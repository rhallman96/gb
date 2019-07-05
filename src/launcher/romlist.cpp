#include "romlist.h"

#include <libgen.h>
#include <dirent.h>

// file directory names
const string RomList::c_romDir = "roms/";
const string RomList::c_saveDir = "save/";

RomList::RomList( wxWindow* parent, string directory ) :
    wxListBox( parent, ID ),
    m_romDir( directory + c_romDir ),
    m_saveDir( directory + c_saveDir )
{
    wxFont listFont(16, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    this->SetFont( listFont );
    this->populate();
}

RomList::~RomList( void )
{
    // delete ROMs in map
    using std::pair;

    for( pair<string, Rom*> element : m_romMap ) {
	delete element.second;
    }
}

Rom* RomList::getSelection( void )
{
    int index = this->GetSelection();
    if( index == wxNOT_FOUND ) { return NULL; }

    wxString label = this->GetString( index );
    string name = label.ToStdString();

    if( m_romMap.find( name ) == m_romMap.end() )
    {
	return NULL;
    }
    
    Rom* rom = m_romMap[ name ];
    return rom;
}

void RomList::populate( void )
{
    DIR* dir = opendir( m_romDir.c_str() );
    struct dirent* ent;    

    if( dir != NULL )
    {
	// loop over files entries in directory
	while( (ent = readdir( dir )) != NULL )
	{
	    string filename( ent->d_name );
	    size_t pos = filename.find_last_of(".");
	    
            // check if item is a file ending in .gb
	    string extension = filename.substr( pos );
	    
	    if( extension == Rom::c_romExtension )
	    {
		string name = filename.substr( 0, pos );

		// Add the ROM to the ROM map and the list
		if( m_romMap.find( name ) == m_romMap.end() )
		{    
		    string romPath = m_romDir + filename;
		    string savePath = m_saveDir + name + Rom::c_saveExtension;
		    Rom* rom = new Rom( name, romPath, savePath );
		    m_romMap[ name ] = rom;
		    this->Insert( name, 0 );
		}
	    }
	}
    }
}
