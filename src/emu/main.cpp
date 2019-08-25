#include "window.h"
#include "gb/gb.h"
#include "gb/rom/rom.h"

#include <libgen.h>

#include <string>
using std::string;

// get the program's base directory
string getBaseDir( char* argv0 )
{
    char* buffer = NULL;
    buffer = realpath( argv0, NULL );
    string dir = string( dirname(buffer) ) + "/";
    free( buffer );

    return dir;
}

int main( int argc, char* argv[] )
{
    // parse command line args
    if( argc <= 1 ) { return 1; }
    string path = argv[1];
    string save = "";
    if( argc >= 3 ) { save = argv[2]; }

    // load the ROM
    Rom* rom = Rom::Load( path, save );
    if( rom == NULL ) { return 1; }

    // initialize SDL
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO );
    
    // get the program's base directory (for config files)
    string baseDir = getBaseDir( argv[0] );

    //  start the emulator
    GB* gb = new GB( rom, baseDir );
    Window* window = new Window( gb );
    window->run();

    // clean up
    delete window; window = NULL;
    delete gb; gb = NULL;
    delete rom; rom = NULL;
    return 0;
}
