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
#define DistanceBetweenCells (float)1
#define cellRowSize    (StageWidth/cellSize)
#define cellColumSize (StageHeight/cellSize)

#define min(a,b) ((a<b)?a:b)

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
    float* Vectors[4];

    int NieghborNumber=0;
    bool IsWall =false;
    float divergence =0;
    double pressure =0;

    inline uint32_t color (){
        int r =0;
        int g =0;
        int b =0;
        r=min(255,20* (int)sqrtf((*Vectors[0] + *Vectors[3]) * (*Vectors[0] + *Vectors[3]) +
                               (*Vectors[1] + *Vectors[2]) * (*Vectors[1] + *Vectors[2])));

        g = min(255,20*abs((int)*Vectors[0] + *Vectors[3]));

        b = min(255,20*(int) abs(divergence));

        return RGBcolor( r,g,b);
        };
};

Matrix<Particle> StageMatrix(cellRowSize,cellColumSize);

Matrix<float> XvelocityMatrix(cellRowSize+1,cellColumSize) ;
Matrix<float> YvelocityMatrix(cellRowSize,cellColumSize+1) ;

Particle nothingParticle;
bool init();
bool close();
bool DrawToScreen();      
void UpdateParticles();
bool quit = false;
bool pause = false;
bool step = false;
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
            pause = !pause;
                cout<<"SpaceDown\n";
                break;
             case SDLK_RIGHT:
            pause = true;
            step = true;
                cout<<"Right-Step\n";
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
        cout<< StageMatrix.getPointer(mouse.cellX(),mouse.cellY())->NieghborNumber << " Nieghbors\n";
        cout<< *StageMatrix.getPointer(mouse.cellX(),mouse.cellY())->Vectors[0] << " Left \n";
        cout<< *StageMatrix.getPointer(mouse.cellX(),mouse.cellY())->Vectors[1] << " Up   \n";
        cout<< *StageMatrix.getPointer(mouse.cellX(),mouse.cellY())->Vectors[2] << " Down \n";
        cout<< *StageMatrix.getPointer(mouse.cellX(),mouse.cellY())->Vectors[3] << " Right\n";
        cout<< StageMatrix.getPointer(mouse.cellX(),mouse.cellY())->divergence << "divergence \n";
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

if(!pause || step ){
UpdateParticles();step = false;}

DrawToScreen();

StageTime++;
   }


close();

return 0;
}


bool init (){
//init Matrix
int x,y;
int Xoffset, Yoffset;
        for ( x = 0 ; x < StageMatrix.width; x++)
    {
           for ( y = 0; y < StageMatrix.height; y++)
           {
            Particle* CurrentParticle =  StageMatrix.getPointer(x,y);

            CurrentParticle->index = (y*StageMatrix.width)+x;

            int neighborIndex =0;
            
            // Go through all 4 nieghbors 
            for ( Xoffset = -1; Xoffset <= 1; Xoffset++)
            {
                 for ( Yoffset = -1; Yoffset <= 1; Yoffset++)
                {
                if(Xoffset == 0 && Yoffset == 0 ){continue;}
                if(Xoffset != 0 && Yoffset != 0 ){continue;} // iether one has to be 0 to get the 4 nieghbors
            // Go through all 4 nieghbors 

                if(StageMatrix.checkBounds(x+Xoffset,y+Yoffset)){
                    CurrentParticle->Nieghbors[neighborIndex] = StageMatrix.getPointer(x+Xoffset,y+Yoffset);
                    CurrentParticle->NieghborNumber++;
                } else {
                    CurrentParticle->Nieghbors[neighborIndex] = &nothingParticle ;
                }
                neighborIndex++;
            }
            }
            ///////////////////// Nieghbors done
///////////// this alligns with the fallowing
            CurrentParticle->Vectors[0] = XvelocityMatrix.getPointer(x,y); //left
            CurrentParticle->Vectors[1] = YvelocityMatrix.getPointer(x,y); //up
            CurrentParticle->Vectors[2] = YvelocityMatrix.getPointer(x,y+1);//down
            CurrentParticle->Vectors[3] = XvelocityMatrix.getPointer(x+1,y);//right


           }

    }

XvelocityMatrix.clearMatrix();
YvelocityMatrix.clearMatrix();
nothingParticle.IsWall = true;
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

       *(pixels +i) = CurrentParticle->color(); 
    }
    
 
 


SDL_UnlockSurface(BackCanvas);


if(SDL_BlitScaled(BackCanvas,0,ScreenFront,&DestinationRect) == -1){
    std::cout<< "blitFail: "<< SDL_GetError() << '\n'; 
};

SDL_UpdateWindowSurface(window);

return true;
}




void UpdateParticles(){

int deltaX, deltaY;
int currentX, currentY;

// Managing Mouse input data
if (mouse.click && XvelocityMatrix.checkBounds(mouse.cellX(),mouse.cellY()))
{
 for ( deltaX = -3; deltaX <= 3; deltaX++)
  {
    for ( deltaY = -3; deltaY <=3; deltaY++)
    {
        currentX = mouse.cellX()+deltaX;
        currentY = mouse.cellY()+deltaY;
        
   if (XvelocityMatrix.checkBounds(currentX,currentY)){
    *XvelocityMatrix.getPointer(currentX,currentY) += 20;
   }      
    }
    
  }
}

Particle* CurPrt;

////////                    solve for incompressability

///    Find Divergencies
for (int i = 0; i < StageMatrix.width* StageMatrix.height; i++)
    {
        //get the current particle
        CurPrt = StageMatrix.getPointer(i);
    
    CurPrt->divergence = 0;
    for (int n = 0; n < 4; n++)
    {
      if (!CurPrt->Nieghbors[n]->IsWall) {
  CurPrt->divergence += *CurPrt->Vectors[n] * ((n > 1)?-1:1);
        }else{
            *CurPrt->Vectors[n] =0;
    }
    
    }
    }
    ////      Apply changes to vectors
float vectorChange;
    for (int i = 0; i < StageMatrix.width* StageMatrix.height; i++)
    {
        //get the current particle
         CurPrt = StageMatrix.getPointer(i);

        vectorChange =   CurPrt->divergence / (float) CurPrt->NieghborNumber;


if (!CurPrt->Nieghbors[0]->IsWall) {
    *CurPrt->Vectors[0] +=vectorChange *-1; //FLIPING DIRECTION
}///////////////////////////

if (!CurPrt->Nieghbors[1]->IsWall) {
    *CurPrt->Vectors[1] +=vectorChange *-1; //FLIPING DIRECTION
}///////////////////////////

if (!CurPrt->Nieghbors[2]->IsWall) {
    *CurPrt->Vectors[2] -=vectorChange *-1; //FLIPING DIRECTION
}///////////////////////////

if (!CurPrt->Nieghbors[3]->IsWall) {
    *CurPrt->Vectors[3] -=vectorChange *-1; //FLIPING DIRECTION
}///////////////////////////

    }

/// advecation
float* CurFloat;
int x,y;

for ( x = 0; x < XvelocityMatrix.width; x++)
{
    for ( y = 0; y < XvelocityMatrix.height; y++)
       {  CurFloat = XvelocityMatrix.getPointer(x,y);

    // find my current velocity at this point
        float thisVx = *CurFloat;
        float thisVy = 0;
        int nieghbors=0;
      if(YvelocityMatrix.checkBounds(x,  y  )) { thisVy+= YvelocityMatrix.getValue(x  ,y  );nieghbors+=1 ;} ;
      if(YvelocityMatrix.checkBounds(x-1,y+1)) { thisVy+= YvelocityMatrix.getValue(x-1,y+1);nieghbors+=1 ;} ;
      if(YvelocityMatrix.checkBounds(x,  y+1)) { thisVy+= YvelocityMatrix.getValue(x  ,y+1);nieghbors+=1 ;} ;
      if(YvelocityMatrix.checkBounds(x-1,y  )) { thisVy+= YvelocityMatrix.getValue(x-1,y  );nieghbors+=1 ;} ;
    thisVy = thisVy / nieghbors;

    //point x is where 
    float BackTraceX = x - thisVx;
    float BackTraceY = y - thisVy;

    // Find theX velocty at point x 
    float VectorUL = ( XvelocityMatrix.checkBounds(((int)BackTraceX  ),(int)BackTraceY  ))?  XvelocityMatrix.getValue(((int)BackTraceX  ), (int)BackTraceY  )  :0  ;
    float VectorDL = ( XvelocityMatrix.checkBounds(((int)BackTraceX  ),(int)BackTraceY+1))?  XvelocityMatrix.getValue(((int)BackTraceX  ), (int)BackTraceY+1)  :0  ;
    float VectorUR = ( XvelocityMatrix.checkBounds(((int)BackTraceX+1),(int)BackTraceY  ))?  XvelocityMatrix.getValue(((int)BackTraceX+1), (int)BackTraceY  )  :0  ;
    float VectorDR = ( XvelocityMatrix.checkBounds(((int)BackTraceX+1),(int)BackTraceY+1))?  XvelocityMatrix.getValue(((int)BackTraceX+1), (int)BackTraceY+1)  :0  ;
    float OffsetX = BackTraceX - ((int) BackTraceX);
    float OffsetY = BackTraceY - ((int) BackTraceY);

    // that is now velocity mine
    float W00 = 1-OffsetX/DistanceBetweenCells;
    float W10 = 1-OffsetY/DistanceBetweenCells;
    float W01 = OffsetX/DistanceBetweenCells;
    float W11 = OffsetY/DistanceBetweenCells;

*CurFloat = (W00*W10*VectorUL)+(W01*W10* VectorUR)+ (W01*W11*VectorDL) + (W00*W11*VectorDR);
    }
    
}


// now do alllll of that agian with the y values
for ( x = 0; x < YvelocityMatrix.width; x++)
{
    for ( y = 0; y < YvelocityMatrix.height; y++)
       {  CurFloat = YvelocityMatrix.getPointer(x,y);

    // find my current velocity at this point
        float thisVx = 0;
        float thisVy = *CurFloat;
        int nieghbors=0;

      if(XvelocityMatrix.checkBounds(x  ,y-1)) { thisVx+= XvelocityMatrix.getValue(x  ,y-1)  ;nieghbors+=1;} ;
      if(XvelocityMatrix.checkBounds(x+1,y-1)) { thisVx+= XvelocityMatrix.getValue(x+1,y-1)  ;nieghbors+=1;} ;
      if(XvelocityMatrix.checkBounds(x  ,y  )) { thisVx+= XvelocityMatrix.getValue(x  ,y  )  ;nieghbors+=1;} ;
      if(XvelocityMatrix.checkBounds(x+1,y  )) { thisVx+= XvelocityMatrix.getValue(x+1,y  )  ;nieghbors+=1;} ;
    thisVx =  thisVx/nieghbors;

    //point x is where 
    float BackTraceX = x - thisVx;
    float BackTraceY = y - thisVy;

    // Find theX velocty at point x 
    float VectorUL = ( YvelocityMatrix.checkBounds(((int)BackTraceX),   (int)BackTraceY)  )?  YvelocityMatrix.getValue(((int)BackTraceX  ),(int)BackTraceY)  :0;
    float VectorDL = ( YvelocityMatrix.checkBounds(((int)BackTraceX),   (int)BackTraceY+1))?  YvelocityMatrix.getValue(((int)BackTraceX  ),(int)BackTraceY+1):0;
    float VectorUR = ( YvelocityMatrix.checkBounds(((int)BackTraceX+1), (int)BackTraceY)  )?  YvelocityMatrix.getValue(((int)BackTraceX+1),(int)BackTraceY)  :0;
    float VectorDR = ( YvelocityMatrix.checkBounds(((int)BackTraceX+1), (int)BackTraceY+1))?  YvelocityMatrix.getValue(((int)BackTraceX+1),(int)BackTraceY+1):0;
    float OffsetX = BackTraceX - ((int) BackTraceX);
    float OffsetY = BackTraceY - ((int) BackTraceY);

    // that is now velocity mine
    float W00 = 1-OffsetX/DistanceBetweenCells;
    float W10 = 1-OffsetY/DistanceBetweenCells;
    float W01 = OffsetX/DistanceBetweenCells;
    float W11 = OffsetY/DistanceBetweenCells;
*CurFloat = (W00*W10*VectorUL)+(W01*W10* VectorUR)+ (W01*W11*VectorDL) + (W00*W11*VectorDR);
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
