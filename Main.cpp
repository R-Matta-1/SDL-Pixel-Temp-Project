#define SDL_MAIN_HANDLED
#include<SDL.h>
#include<SDL_render.h>
#include"Matrix.h"
#include<math.h>
#include<cstdlib>

#define WindowWidth 1200  
#define WindowHeight 750
#define StageWidth 600
#define StageHeight 750
#define cellSize 5
#define cellRowSize    (StageWidth/cellSize)
#define cellColumSize (StageHeight/cellSize)

#define RGBcolor(R,G,B) ((R << 16) | (G << 8) | B)
using namespace std;


SDL_Window*   window = 0;
SDL_Renderer* renderer = 0;
SDL_Surface*  ScreenFront = 0;
SDL_Surface*  BackCanvas = 0;
SDL_Texture*  Texture = 0;
SDL_Rect DestinationRect ={};
SDL_Event e ;

struct Particle
{
    int index =0;
    Uint8 density =0;
    uint32_t color =0;
};

Matrix<Particle> StageMatrix(cellRowSize,cellColumSize);
bool init();
bool close();
bool DrawToScreen();      
void UpdateParticles();
bool quit = false;
int time = 0;

struct MouseData{
    int x = -1;
    int y = -1;
    bool click =0;
       int cellX() const {
        return (int)(DestinationRect.x + x / cellSize);
    }
       int cellY() const {
        return (int)(DestinationRect.y + y / cellSize);
    }
};
MouseData mouse;

int main(int argc, char* args[] ){
//init
if(!init()){
    std::cout<< "init fail"<< SDL_GetError();
    return 0;}


   while( !quit )
   {
       //Handle events on queue
      while( SDL_PollEvent( &e ) != 0 )
      {
          switch (e.type)
          {
          case SDL_QUIT:
               quit = true;
           break;

////////////////////////  KEYS DOWN
          case SDL_KEYDOWN:
            switch (e.key.keysym.sym)
            {
            case SDLK_SPACE:
                cout<<"SpaceDown\n";
                break;
            
            default:
                break;
            }
            break;
/////////////////////// KEYS DOWN End
        case SDL_MOUSEMOTION:

       mouse.x = e.motion.x; 
       mouse.y = e.motion.y;
cout<< mouse.cellX()<< " , "<<mouse.cellY()<<"\n"<<mouse.click<<"\n";
        break;

        case SDL_MOUSEBUTTONDOWN:
        mouse.click = true;
        break;

        case SDL_MOUSEBUTTONUP:
        mouse.click = false;
          default:
           break;
          }


          if( e.type == SDL_QUIT )
          {
              quit = true;
          }
      }

///////////////   EVENTS End


UpdateParticles();
DrawToScreen();

time++;
   }


close();

return 0;
}


bool init (){
//init Matrix
        for (int x = 0 ; x < StageMatrix.width; x++)
    {
           for (int y = 0; y < StageMatrix.height; y++)
           {
            StageMatrix.getPointer(x,y)->index = (x*StageMatrix.width)+y;
           }

    }

//InitSDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        std::cout<< "\n\ninit Error\n" << SDL_GetError() <<"\n\n\n";
        return 0;
    }

    window = SDL_CreateWindow(
    			"Polar SDL Grahp",
    			SDL_WINDOWPOS_UNDEFINED	,
    			SDL_WINDOWPOS_UNDEFINED	,
    			WindowWidth			,
    			WindowHeight			,
    			SDL_WINDOW_SHOWN 
    		);

    if	( window == NULL )	{
    printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    return 0;
    }

    ScreenFront  = SDL_GetWindowSurface(window);

     BackCanvas  = SDL_CreateRGBSurface(SDL_SWSURFACE ,
        cellRowSize,    
        cellColumSize, 32,
        0xFF0000,   //R
        0x00FF00,   //G
        0x0000FF,   //B
        0x000000);  //A

    DestinationRect.h = StageHeight ;
    DestinationRect.w = StageWidth;
    DestinationRect.x = 0;
    DestinationRect.y = 0;

    return 1;
}

bool DrawToScreen(){

SDL_LockSurface(BackCanvas);
  // Access pixel data
    Uint32* pixels = (Uint32*)BackCanvas->pixels;

 for (int x = 0; x < cellRowSize; x++)
 {
    for (int y = 0; y < cellColumSize; y++)
    {
       Particle* CurrentParticle = StageMatrix.getPointer(x,y);

       *(pixels + CurrentParticle->index) = CurrentParticle->color; 
    }
    
 }
 


SDL_UnlockSurface(BackCanvas);


if(SDL_BlitScaled(BackCanvas,0,ScreenFront,&DestinationRect) == -1){
    std::cout<< "blitFail: "<< SDL_GetError() << '\n'; 
};

SDL_UpdateWindowSurface(window);

return true;
}

void UpdateParticles(){

for (int x = 0; x < StageMatrix.width; x++)
{
    for (int y = 0; y < StageMatrix.height; y++)
    {
        Particle* CurrentParticle = StageMatrix.getPointer(x,y);
        CurrentParticle->color = RGBcolor(x,(x+time),y);
    }
    
}
}

bool close() {
        SDL_DestroyWindow( window );
        SDL_DestroyRenderer(renderer);
 //       SDL_FreeSurface(ScreenFront);
        SDL_FreeSurface(BackCanvas);
        SDL_DestroyTexture(Texture);
    SDL_Quit();
    return 0;
}



