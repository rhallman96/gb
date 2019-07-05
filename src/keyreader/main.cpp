#include "keyreader.h"

int main( int argc, char* argv[] )
{
    if( argc < 2 ) { return 1; }

    string path( argv[1] );
    KeyReader* reader = new KeyReader( path );
    reader->run();
    
    return 0;
}
