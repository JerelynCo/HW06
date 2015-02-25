/*This source code copyrighted by Lazy Foo' Productions (2004-2014)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation how opaque
		void setAlpha( Uint8 alpha );

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The application time based timer
class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};

//The Bird that will move around on the screen
class Bird
{
    public:
		//The dimensions of the Bird
		static const int BIRD_WIDTH = 20;
		static const int BIRD_HEIGHT = 20;

		//Maximum axis velocity of the Bird
		static const int Bird_VEL = 10;

		//Initializes the variables
		Bird();

		//Takes key presses and adjusts the Bird's velocity
		void handleEvent( SDL_Event& e );

		//Moves the Bird
		void move();

		//Shows the Bird on the screen
		void render();

		void rotateBox();

    private:
		//The X and Y offsets of the Bird
		int bPosX, bPosY;

		//The velocity of the Bird
		int mVelX, mVelY;

		int gGravX, gGravY;

		int angle;

		SDL_Rect box;

		int centerX,centerY;
};

class Pipe{
    public:
        static const int Bird_WIDTH = 20;
		static const int Bird_HEIGHT = 20;

		Pipe();

		//Shows the pipe on the screen
		void render();

		int pPosX,pPosY;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gBirdTexture;
LTexture gPipe;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

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
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}


Bird :: Bird()
{
    //Initialize the offsets
    bPosX = 100;
    bPosY = 100;

    //Initialize the velocity
    mVelX = gGravX;
    mVelY = gGravY;

    //Initialize the velocity
    gGravX = 0;
    gGravY = 0;

    angle = -20;

    box.x = bPosX;
    box.y = bPosY;
    box.h = BIRD_HEIGHT;
    box.w = BIRD_WIDTH;


}
Pipe::Pipe()
{
    pPosX = 200;
    pPosY = 400;
}
bool goDown = false;
void Bird::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( (e.type == SDL_KEYDOWN && e.key.repeat == 0)&&(e.key.keysym.sym==SDLK_UP) )
    {
       goDown = true;

    }
    //If a key was released
    else if(( e.type == SDL_KEYUP && e.key.repeat == 0 )&&(e.key.keysym.sym==SDLK_UP))
    {
        //Adjust the velocity
       goDown = false;

    }
}

void Bird::move()
{
    //Move the Bird up or down
    if(goDown == true)
    {
        //10 is gravity

        bPosY = bPosY - mVelY;
        mVelY = mVelY - 1;
        angle -=5;
        if (angle<-20)
        {
            angle = -20;
        }
    }
    else if(( bPosY < 0 ) || ( bPosY + BIRD_HEIGHT > SCREEN_HEIGHT )||goDown == false)
    {

        bPosY = bPosY + mVelY;
        mVelY = mVelY + 1;
        angle +=5;
        if (angle>20)
        {
            angle = 20;
        }
    }
    rotateBox();
}

void Bird::rotateBox()
{
    centerX = box.x+(box.w/2);
    centerY = box.y+(box.h/2);

    int upperRX = centerX +(-box.w/2)*cos(20)-(box.y/2)*sin(20);
    int upperRY = centerY +(-box.w/2)*sin(20)+(box.y/2)*cos(20);

    int lowerRX = centerX +(-box.w/2)*cos(20)-(-box.y/2)*sin(20);
    int lowerRY = centerY +(-box.w/2)*sin(20)+(-box.y/2)*cos(20);

    int upperLX = centerX +(box.w/2)*cos(20)-(box.y/2)*sin(20);
    int upperLY = centerY +(box.w/2)*sin(20)+(box.y/2)*cos(20);

    int lowerLX = centerX +(box.w/2)*cos(20)-(-box.y/2)*sin(20);
    int lowerLY = centerY +(box.w/2)*sin(20)+(-box.y/2)*cos(20);
}

void Bird::render()
{
    //Show the Bird
	 gBirdTexture.render( bPosX, bPosY,NULL,angle );
}

void Pipe::render()
{
    gPipe.render(pPosX,pPosY);
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
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
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

	//Load Bird texture
	if( ! gBirdTexture.loadFromFile( "Images/bird.jpeg" ) )
	{
		printf( "Failed to load Bird texture!\n" );
		success = false;
	}
	if( !gPipe.loadFromFile( "Images/pipe.png" ) )
	{
		printf( "Failed to load Bird texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
    gBirdTexture.free();

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
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

			//The Bird that will be moving around on the screen
		    Bird bird;
            Pipe pipe;
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
					}

					//Handle input for the Bird
				 bird.handleEvent( e );
				}

				//Move the Bird
			 bird.move();

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render objects
			    bird.render();
				pipe.render();

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
