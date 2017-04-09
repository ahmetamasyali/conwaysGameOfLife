#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <windows.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SQUARE_SIZE = 20;
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;


SDL_Texture* gTextureBlack = NULL;
SDL_Texture* gTextureWhite = NULL;



int main( int argc, char* args[] )
{

    ShowWindow( GetConsoleWindow(), SW_HIDE ); //hide console
    int columnCount = SCREEN_WIDTH/SQUARE_SIZE;
    int lineCount = SCREEN_HEIGHT/SQUARE_SIZE;

	int matrix[SCREEN_WIDTH/SQUARE_SIZE][SCREEN_HEIGHT/SQUARE_SIZE];
	int previousStateMatrix[SCREEN_WIDTH/SQUARE_SIZE][SCREEN_HEIGHT/SQUARE_SIZE];

	int i,k,l,m;

    for(i=0;i<columnCount;i++){
        for(k=0;k<lineCount;k++){
            matrix[i][k]= 0;
        }
    }
    matrix[13][13]= 1;
    matrix[13][14]= 1;
    matrix[13][15]= 1;
    matrix[14][14]= 1;
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;
            bool lifeGoesOn = true;;
			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}//User presses a key
					else if( e.type == SDL_KEYDOWN ) {
                            //Select surfaces based on key press
                            switch( e.key.keysym.sym ) {
                                case SDLK_SPACE:
                                lifeGoesOn = !lifeGoesOn;
                                break;
                            }
					}else if( e.type == SDL_MOUSEBUTTONDOWN ) {
					     //Get mouse position
					     int x, y; SDL_GetMouseState( &x, &y );
					     matrix[x/SQUARE_SIZE][y/SQUARE_SIZE] =  matrix[x/SQUARE_SIZE][y/SQUARE_SIZE] == 1 ? 0 : 1;
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				for(i=0;i<columnCount;i++){
                    for(k=0;k<lineCount;k++){
                            SDL_Rect viewPort;
                                 viewPort.x = i*SQUARE_SIZE;;
                                 viewPort.y = k*SQUARE_SIZE;
                                 viewPort.w = SQUARE_SIZE;
                                 viewPort.h = SQUARE_SIZE;
                                 SDL_RenderSetViewport( gRenderer, &viewPort);
                            if(matrix[i][k] == 1){
                                SDL_RenderCopy( gRenderer, gTextureBlack, NULL, NULL );
                            }else{
                                SDL_RenderCopy( gRenderer, gTextureWhite, NULL, NULL );
                            }

                    }
                }

				//Update screen
				SDL_RenderPresent( gRenderer );

                if(lifeGoesOn){
                    for(i=0;i<columnCount;i++){
                        for(k=0;k<lineCount;k++){
                            previousStateMatrix[i][k] = matrix[i][k];
                        }
                    }

                    for(i=0;i<columnCount;i++){
                        for(k=0;k<lineCount;k++){
                                int liveNeighborCount = 0;
                                for(l=i-1;l<i+2;l++){
                                    for(m=k-1;m<k+2;m++){
                                            if(l>=0 && m>=0 && m<columnCount && l<columnCount && previousStateMatrix[l][m] == 1){
                                                if(i != l || k!=m){
                                                    liveNeighborCount++;
                                                }
                                            }
                                    }
                                }
                                if(previousStateMatrix[i][k] == 1){
                                    if(liveNeighborCount < 2 || liveNeighborCount > 3){
                                        matrix[i][k] = 0; // DIED ,BECAUSE OF CROWD
                                     }
                                }else{
                                    if(liveNeighborCount == 3){
                                        matrix[i][k] = 1; // ALIVE!
                                     }
                                }

                        }
                    }
                 }

                Sleep(300);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Game Of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load texture
	gTextureBlack = loadTexture( "black.png" );
	gTextureWhite = loadTexture( "white.png" );
	if( gTextureBlack == NULL || gTextureWhite == NULL)
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}

	//Nothing to load
	return success;
}

void close()
{
	//Free loaded image
	SDL_DestroyTexture( gTextureWhite );
	SDL_DestroyTexture( gTextureBlack );
	gTextureWhite = NULL;
    gTextureBlack = NULL;

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture( std::string path )
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}
