/***for controls press and release the up botton don't hold it down or else the burd will fall***/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <cmath>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

#define PI 3.14159265

//Screen dimension constants
const int SCOREBOARD_HEIGHT = 50;
const int PLAYFIELD_HEIGHT = 430;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = SCOREBOARD_HEIGHT+PLAYFIELD_HEIGHT;

//Texture wrapper class
class LTexture{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile(std::string);

		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText(std::string, SDL_Color);
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor(Uint8, Uint8, Uint8);

		//Set blending
		void setBlendMode(SDL_BlendMode);

		//Set alpha modulation how opaque
		void setAlpha(Uint8);

		//Renders texture at given point
		void render(int, int, SDL_Rect* = NULL, double = 0.0, SDL_Point* = NULL, SDL_RendererFlip = SDL_FLIP_NONE);

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
class LTimer{
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
class Bird{
    public:
		//The dimensions of the Bird
		static const int BIRD_WIDTH = 40;
		static const int BIRD_HEIGHT = 40;
		SDL_Point* axis[2];
		SDL_Point* center;

		//Initializes the variables
		Bird():
			mPosX(SCREEN_WIDTH/2), mPosY(SCOREBOARD_HEIGHT+PLAYFIELD_HEIGHT/2), mVelY(0), angle(0),
			axis{new SDL_Point{0, 0}, new SDL_Point{0, 0}}, center(new SDL_Point{0, 0}) {};

		void handleEvent(SDL_Event&);
		bool fall();
		void render();
		
    private:
		//The X and Y offsets of the Bird
		double mPosX, mPosY;

		//The velocity of the Bird
		double mVelY;
		const double mVelX = 5;

		double angle;
		
		void flap();
};

struct Pipe{
	int PIPE_WIDTH = 80;
	int PIPE_HEIGHT;
	SDL_Point* axis[2];
	SDL_Point* center;

	Pipe(int x, int y, int h, double a):
		mPosX(x), mPosY(y), angle(a), PIPE_HEIGHT(h),
		axis{new SDL_Point{0, 0}, new SDL_Point{0, 0}}, center(new SDL_Point{0, 0}) {};
	
	bool move();
	void render();
	
	private:
		int mPosX, mPosY;
		double angle;
};

SDL_Point* add(int, SDL_Point*, int, SDL_Point*);
bool SAT(Bird&, Pipe&);
double dot(SDL_Point*, SDL_Point*);

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

int gScore = 0;

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Global Font
TTF_Font* gFont =  NULL;

//Scene textures
LTexture gBirdTexture;
LTexture gPipeTexture;
LTexture gBGTexture;

//Scoreboard textures
LTexture gScoreTexture, gScoreTextTexture;

//Global timer
LTimer gTimer;



int main(int argc, char* args[]){
	//Start up SDL and create window
	if(!init()){
		printf("Failed to initialize!\n");
	}else{
		//Load media
		if(!loadMedia()){
			printf("Failed to load media!\n");
		}else{
			//Main loop flag
			bool quit = false;
			bool gameover = false;

			//Event handler
			SDL_Event e;

			//The background scrolling offset
			int scrollingOffset = 0;

			std::stringstream scoreText;

			//Objects
		    Bird bird;

            std::vector<Pipe> btmPipe;
            std::vector<Pipe> topPipe;

            int i = 0;
            int j = 0;

			SDL_Rect scoreboard = {0, 0, SCREEN_WIDTH, SCOREBOARD_HEIGHT};
			SDL_Rect playfield = {0, SCOREBOARD_HEIGHT, SCREEN_WIDTH, PLAYFIELD_HEIGHT};

            //start global game timer
            gTimer.start();
			//While application is running
			while(!quit){
				//Handle events on queue
				while(SDL_PollEvent(&e) != 0){
					//User requests quit
					if(e.type == SDL_QUIT){
						quit = true;
					}else if(e.type == SDL_KEYDOWN){
                        //Pause/unpause
                        if(e.key.keysym.sym == SDLK_p){
                            if(gTimer.isPaused()){
                                gTimer.unpause();
                            }else{
                                gTimer.pause();
                            }
                        }
                    }
					//Handle input for the Bird
					if(!gameover){bird.handleEvent(e);}
				}
                
				if(gTimer.isStarted() && !gTimer.isPaused()){
                    //Move the Bird
                    bird.fall();

                    //Temporary generation of pipes
                    if(i%50 == 0){
                        btmPipe.emplace_back(800, PLAYFIELD_HEIGHT-100, 100, 0);
                    }
                    
					++i;

                    if(j%50 == 0){
                        topPipe.emplace_back(800, 0, 100, 180);
                    }
                    
					++j;
                }
         	
				//Clear screen
				SDL_RenderClear(gRenderer);

				SDL_RenderSetViewport(gRenderer, &scoreboard);
				SDL_SetRenderDrawColor(gRenderer, 0xB5, 0xBF, 0xFF, 0xFF);

				scoreText.str("");
				scoreText << gScore;

				//Render text
				SDL_Color textColor = {0x00, 0x00, 0x00, 0xFF};
				if(!gScoreTexture.loadFromRenderedText(scoreText.str().c_str(), textColor)){
					printf("Unable to render score texture!\n");
				}

				//Render textures
				gScoreTextTexture.render((SCREEN_WIDTH-gScoreTextTexture.getWidth())/2, SCOREBOARD_HEIGHT-gScoreTextTexture.getHeight());
				gScoreTexture.render((SCREEN_WIDTH/2+gScoreTextTexture.getWidth()/2), SCOREBOARD_HEIGHT-gScoreTexture.getHeight());
				gBirdTexture.render(0,0);

				SDL_RenderSetViewport(gRenderer, &playfield);
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xBF, 0xFF );
				
				//Scroll background
				if(!gameover){--scrollingOffset;}
				
				if(scrollingOffset < -gBGTexture.getWidth()){
					scrollingOffset = 0;
				}
				//Render background
				gBGTexture.render(scrollingOffset, 0);
				gBGTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);
				gBGTexture.render(scrollingOffset + 2*gBGTexture.getWidth(), 0);
				
				/**renders and deletes objects**/
				for(auto &Pipe : btmPipe){
					if(!gameover){
						if(!Pipe.move()){
							btmPipe.erase(btmPipe.begin());
						}
					}
					if(SAT(bird, Pipe)){gameover = true;}
					Pipe.render();
				}
				for(auto &Pipe : topPipe){
					if(!gameover){
						if(!Pipe.move()){
							topPipe.erase(topPipe.begin());
						}
					}
					if(SAT(bird, Pipe)){gameover = true;}
					Pipe.render();
				}

				if(!bird.fall()){gameover = true;}
				bird.render();
				
				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	//Free resources and close SDL
	close();
	return 0;
}


LTexture::LTexture(){
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture(){
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path){
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if(loadedSurface == NULL){
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if(newTexture == NULL){
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

LTimer::LTimer(){
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start(){
    //Start the timer
    mStarted = true;
    //Unpause the timer
    mPaused = false;
    //Get the current clock time
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void LTimer::stop(){
    //Stop the timer
    mStarted = false;
    //Unpause the timer
    mPaused = false;
    //Clear tick variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

void LTimer::pause(){
	//If the timer is running and isn't already paused
    if(mStarted && !mPaused){
    //Pause the timer
        mPaused = true;
        //Calculate the paused ticks
        mPausedTicks = SDL_GetTicks()-mStartTicks;
        mStartTicks = 0;
    }
}

void LTimer::unpause(){
    //If the timer is running and paused
    if(mStarted && mPaused){
        //Unpause the timer
        mPaused = false;
        //Reset the startng ticks
        mStartTicks = SDL_GetTicks()-mPausedTicks;
        //Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks(){
	//The actual timer time
    Uint32 time = 0;
	
	//If the timer is running
    if(mStarted){
    //If the timer is paused
        if(mPaused){
            //Return the number of ticks when the timer is paused
            time = mPausedTicks;
        }else{
            //Return the current time minus the start time
            time = SDL_GetTicks()-mStartTicks;
        }
    }
    return time;
}

bool LTimer::isStarted(){
    //Timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused(){
    //Timer is running and paused
    return mPaused && mStarted;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor){
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if( textSurface != NULL ){
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if( mTexture == NULL ){
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free(){
	//Free texture if it exists
	if(mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending){
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha){
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
	//Set rendering space and render to screen
	SDL_Rect renderQuad = {x, y, mWidth, mHeight};

	//Set clip rendering dimensions
	if(clip != NULL){
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth(){
	return mWidth;
}

int LTexture::getHeight(){
	return mHeight;
}

void Bird::handleEvent(SDL_Event& e){
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_SPACE){
       flap();
    }
}

void Bird::flap(){
	mVelY = -5;
}

bool Bird::fall(){
	for(int i = 0; i < 2; ++i){
		axis[i]->x = cos((angle+90*i)*PI/180);
		axis[i]->y = sin((angle+90*i)*PI/180);
	}
	
	center->x = mPosX+BIRD_WIDTH/2;
	center->y = mPosY+BIRD_HEIGHT/2;
	
	mVelY += 0.1;
	mPosY += mVelY;
	
	if(mPosY < 0 || mPosY+BIRD_HEIGHT > PLAYFIELD_HEIGHT){
		return false;
	}
	
	return true;
	
}

void Bird::render(){
	angle = 180*atan(mVelY/mVelX)/PI;
	gBirdTexture.render(mPosX, mPosY, NULL, angle);
}

bool Pipe::move(){
	for(int i = 0; i < 2; ++i){
		axis[i]->x = cos((angle+90*i)*PI/180);
		axis[i]->y = sin((angle+90*i)*PI/180);
	}
	
	center->x = mPosX+PIPE_WIDTH/2;
	center->y = mPosY+PIPE_HEIGHT/2;
	
    mPosX = mPosX - 10;
	if(mPosX+PIPE_WIDTH < 0){
		return false;
	}
	
	return true;
}

void Pipe::render(){
    gPipeTexture.render(mPosX, mPosY, new SDL_Rect{0, 0, gPipeTexture.getWidth(), PIPE_HEIGHT}, angle);
}

bool SAT(Bird& b, Pipe& p){
	for(int i = 0; i < 2; ++i){
		if(dot(b.axis[i], add(b.BIRD_WIDTH/2, b.axis[0], b.BIRD_HEIGHT/2, b.axis[1]))+
			dot(b.axis[i], add(p.PIPE_WIDTH/2, p.axis[0], p.PIPE_HEIGHT/2, p.axis[1])) <
			dot(b.axis[i], new SDL_Point{p.center->x-b.center->x, p.center->y-b.center->y})){
			return false;
		}
	}
	
	for(int i = 0; i < 2; ++i){
		if(dot(p.axis[i], add(b.BIRD_WIDTH/2, b.axis[0], b.BIRD_HEIGHT/2, b.axis[1]))+
			dot(p.axis[i], add(p.PIPE_WIDTH/2, p.axis[0], p.PIPE_HEIGHT/2, p.axis[1])) <
			dot(p.axis[i], new SDL_Point{p.center->x-b.center->x, p.center->y-b.center->y})){
			return false;
		}
	}
	
	return true;
}

SDL_Point* add(int m1, SDL_Point* p1, int m2, SDL_Point* p2){
	return new SDL_Point{m1*p1->x+m2*p2->x, m1*p1->y+m2*p2->y};
}

double dot(SDL_Point* p1, SDL_Point* p2){
	return std::abs(p1->x*p2->x+p1->y*p2->y);
}

bool init(){
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ){
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL ){
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )	{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) ){
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
				//Initialize SDL_ttf
				if(TTF_Init() == -1){
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}
	return success;
}

bool loadMedia(){
	//Loading success flag
	bool success = true;
	//Load font
	gFont = TTF_OpenFont("Assets/ostrich.ttf", 50);
	if(gFont == NULL){
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else{
		SDL_Color textColor = {0x00, 0x00, 0x00, 0xFF};
		//Load 'score' texture
		if(!gScoreTextTexture.loadFromRenderedText("SCORE:", textColor)){
			printf("Unable to render score texture!\n");
			success = false;
		}
	}
	//Load textures
	if(!gBGTexture.loadFromFile("Assets/background.png")){
		printf("Failed to load bg texture!\n");
		success = false;
	}
	if(!gBirdTexture.loadFromFile("Assets/bird.png")){
		printf("Failed to load bird texture!\n");
		success = false;
	}
	if(!gPipeTexture.loadFromFile("Assets/pipe.png")){
		printf("Failed to load pipe texture!\n");
		success = false;
	}
	return success;
}

void close(){
	//Free loaded images
    gBirdTexture.free();
    gPipeTexture.free();
    gBGTexture.free();

    TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}