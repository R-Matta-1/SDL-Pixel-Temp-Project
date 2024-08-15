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

using namespace std;


SDL_Window*   window = 0;
SDL_Renderer* renderer = 0;
SDL_Surface*  ScreenFront = 0;
SDL_Surface*  BackCanvas = 0;
SDL_Texture*  Texture = 0;
SDL_Rect DestinationRect ={};
SDL_Event e ;

inline uint32_t RGBcolor(int R,int G,int B){return (((R) << 16) | ((G) << 8) | (B));}


struct Particle
{
    int index =0;
    Particle* Nieghbors[4] ;
    int NieghborNumber=0;
    bool IsWall =0;
    double pressure =0;
    

    inline uint32_t color (){
        return RGBcolor(
            NieghborNumber*62 ,
            NieghborNumber*62 ,
            NieghborNumber*62 );};
};

Matrix<Particle> StageMatrix(cellRowSize,cellColumSize);
Matrix<float> XvelocityMatrix((cellRowSize),cellColumSize) ;
Matrix<float> YvelocityMatrix((cellRowSize),cellColumSize) ;
Particle nothingParticle;
bool init();
bool close();
bool DrawToScreen();      
void UpdateParticles();
bool quit = false;
int StageTime = 0;

struct MouseData{
    int x = -1;
    int y = -1;
    bool click = false;

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
       
        break;

        case SDL_MOUSEBUTTONDOWN:
        mouse.click = true;
            if (StageMatrix.checkBounds(mouse.cellX(),mouse.cellY()) )
            {
        cout<< StageMatrix.getPointer(mouse.cellX(),mouse.cellY())->NieghborNumber;
            }
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

StageTime++;
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
            Particle* CurrentParticle =  StageMatrix.getPointer(x,y);

            CurrentParticle->index = (y*StageMatrix.width)+x;

            int neighborIndex =0;
            for (int Xoffset = -1; Xoffset <= 1; Xoffset++)
            {
                 for (int Yoffset = -1; Yoffset <= 1; Yoffset++)
            {
                if(Xoffset == 0 && Yoffset == 0 ){continue;}
                if(Xoffset != 0 && Yoffset != 0 ){continue;} // iether one has to be 0 to get the 4 nieghbors

                if(StageMatrix.checkBounds(x+Xoffset,y+Yoffset)){
                    CurrentParticle->Nieghbors[neighborIndex] = StageMatrix.getPointer(x+Xoffset,y+Yoffset);
                    CurrentParticle->NieghborNumber++;
                } else {
                    CurrentParticle->Nieghbors[neighborIndex] = &nothingParticle ;
                }
                neighborIndex++;
            }
            }
            


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

 for (int i = 0; i < cellRowSize* cellColumSize; i++)
    {
       Particle* CurrentParticle = StageMatrix.getPointer(i);

       *(pixels +i) = CurrentParticle->color; 
    }
    
 
 


SDL_UnlockSurface(BackCanvas);


if(SDL_BlitScaled(BackCanvas,0,ScreenFront,&DestinationRect) == -1){
    std::cout<< "blitFail: "<< SDL_GetError() << '\n'; 
};

SDL_UpdateWindowSurface(window);

return true;
}

void UpdateParticles(){

// Managing input data
if (mouse.click && StageMatrix.checkBounds(mouse.cellX(),mouse.cellY()))
{
  for (int deltaX = -5; deltaX < 5; deltaX++)
  {
    for (int deltaY = -5; deltaY < 5; deltaY++)
    {
   if (StageMatrix.checkBounds(mouse.cellX()+deltaX,mouse.cellY()+deltaY))
   {
    StageMatrix.getPointer(mouse.cellX()+deltaX,mouse.cellY()+deltaY) ->pressure+=1;
   }      
    }
    
  }
}


//////// solve for incompressability
for (int x = 0; x < StageMatrix.width; x++)
{
    for (int y = 0; y < StageMatrix.height; y++)
    {
            Particle* currentParticle = StageMatrix.getPointer(x,y);
        
        //currentParticle->pressure+= sin(x/5)+ cos(y/5);
            
    }
    
}
////////////////////// defuse
for (int x = 0; x < StageMatrix.width; x++)
{
    for (int y = 0; y < StageMatrix.height; y++)
    {

            
    }
    
}

}

bool close() {
        SDL_DestroyWindow( window );
        SDL_DestroyRenderer(renderer);
 //       SDL_FreeSurface(ScreenFront); //because it was cleared when window was
        SDL_FreeSurface(BackCanvas);
        SDL_DestroyTexture(Texture);
    SDL_Quit();
    return 0;
}
