#include "window.h"
#include "gb/gb.h"
#include "gb/rom/rom.h"

#include <string>

int main( int argc, char* argv[] )
{
    using std::string;
    
    if( argc <= 1 ) { return 1; }

    string path = argv[1];
    string save = "";
    
    if( argc >= 3 ) { save = argv[2]; }
    
    Rom* rom = Rom::Load( path, save );

    if( rom == NULL ) { return 1; }
    
    GB* gb = new GB( rom );
    Window* window = new Window( gb );

    window->run();

    delete window; window = NULL;
    delete gb; gb = NULL;
    delete rom; rom = NULL;
    return 0;
}
