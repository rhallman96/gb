#include "keyreader.h"
#include "../buttons.h"

const string KeyReader::c_iconDir = "icon/";

KeyReader::KeyReader( string path ) :
    m_state( STATE_A ),
    mp_config( new Config( path ) )
{
    SDL_Init( SDL_INIT_VIDEO );
    SDL_CreateWindowAndRenderer( c_width,
				 c_height,
				 SDL_WINDOW_INPUT_GRABBED,
				 &mp_window,
				 &mp_renderer );

    this->loadStateImages( c_iconDir );
}

KeyReader::~KeyReader( void )
{
    if( mp_config != NULL )
    {
	delete mp_config;
	mp_config = NULL;
    }

    for( int i = 0; i < STATE_DONE; i++ )
    {
	if( m_stateImages[ i ] != NULL ) {
	    SDL_DestroyTexture( m_stateImages[ i ] );
	}
    }
    
    SDL_DestroyWindow( mp_window ); mp_window = NULL;
    SDL_DestroyRenderer( mp_renderer ); mp_renderer = NULL;

    IMG_Quit();
    SDL_Quit();
}

void KeyReader::run( void )
{
    SDL_Event e;
    bool done = false;

    while( done == false )
    {
	// Poll window events
	while( SDL_PollEvent( &e ) )
	{
	    if( e.type == SDL_QUIT ) { done = true; }
	    switch( e.type )
	    {
                case SDL_QUIT:
		    done = true;
		    break;
	        case SDL_KEYDOWN:
		    this->handleKeyPress( e.key );
		    break;
	        default:
		    break;
	    }
	}

	this->render();
	
	if( m_state == STATE_DONE )
	{
	    mp_config->write();
	    done = true;
	}
    }
}

void KeyReader::loadStateImages( string path )
{
    m_stateImages[ STATE_A ] = this->loadImage( path + "a.png" );
    m_stateImages[ STATE_B ] = this->loadImage( path + "b.png" );
    m_stateImages[ STATE_START ] = this->loadImage( path + "start.png" );
    m_stateImages[ STATE_SELECT ] = this->loadImage( path + "select.png" );
    m_stateImages[ STATE_LEFT ] = this->loadImage( path + "left.png" );
    m_stateImages[ STATE_RIGHT ] = this->loadImage( path + "right.png" );
    m_stateImages[ STATE_UP ] = this->loadImage( path + "up.png" );
    m_stateImages[ STATE_DOWN ] = this->loadImage( path + "down.png" );
}

SDL_Texture* KeyReader::loadImage( string path )
{
    SDL_Texture* texture = NULL;
    SDL_Surface* loaded = IMG_Load( path.c_str() );

    if( loaded != NULL )
    {
	texture = SDL_CreateTextureFromSurface( mp_renderer, loaded );
	SDL_FreeSurface( loaded );
    }
    
    return texture;
}

void KeyReader::handleKeyPress( SDL_KeyboardEvent& e )
{
    if( m_state == STATE_DONE ) { return; }
    
    int scanCode = e.keysym.scancode;
    string button = this->getButtonValue();

    mp_config->addMapping( button, scanCode );
    m_state++;
}

void KeyReader::render( void )
{
    if( m_state >= STATE_DONE ) { return; }
    
    SDL_Texture* image = m_stateImages[ m_state ];

    SDL_RenderCopy( mp_renderer, image, NULL, NULL );
    SDL_RenderPresent( mp_renderer );
}

string KeyReader::getButtonValue( void )
{
    switch( m_state )
    {
    case STATE_A:
	return BTN_A;
    case STATE_B:
	return BTN_B;
    case STATE_START:
	return BTN_START;
    case STATE_SELECT:
	return BTN_SELECT;
    case STATE_LEFT:
	return BTN_LEFT;
    case STATE_RIGHT:
	return BTN_RIGHT;
    case STATE_UP:
	return BTN_UP;
    case STATE_DOWN:
	return BTN_DOWN;
    default:
	return "";
    }
}
