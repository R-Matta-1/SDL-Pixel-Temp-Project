#include<iostream>
#define Index(x,y) (y*width)+x
#define Coords(i) ((int)i/width,i%width )

template <typename Type>
class Matrix
{
private: // I don't think widthLayer should be private because of Get Pointer but tbh i have no idea how that would
public:
int width, height; 
Type* WidthLayer ;

    Matrix(int width, int height);
    Type getValue(int x, int y);
    Type getValue(int i);
    
    void setValue(int x, int y, Type input);
    void setValue(int i, Type input);

    Type *getPointer(int x, int y);
    Type *getPointer(int i);

    bool checkBounds(int x, int y);
    bool checkBounds(int i);

    int getX(int i);
    int getY(int i);
    int getIndex(int x , int y);
    // and all index variants 
 
    void clearMatrix();
    ~Matrix();

};


template <typename Type>

Matrix<Type>::Matrix(int width, int height) 
    : width(width), height(height)
{
   WidthLayer = new Type [width*height];

};

template <typename Type>
int Matrix<Type>::getX(int i) { return (int)i/width};

template <typename Type>
int Matrix<Type>::getY(int i) { return i%width};

template <typename Type>
int Matrix<Type>::getIndex(int x, int y) { return (y*width)+x};


//////////////////////////// BOUNDS
template <typename Type>
bool Matrix<Type>::checkBounds(int x, int y) {

    return (x >=0 && y >=0 && x < width && y < height );
}
template <typename Type>
bool Matrix<Type>::checkBounds(int i) {

    return (i>=0 && i< ((width*height)) );
}
///////////////////////////////

template <typename Type>
void Matrix<Type>::clearMatrix() {

    for (int i = 0; i < width*height; i++)
    {

            WidthLayer[i] = (Type)0;
        
        
    }

}

/////////////////////////////// THE GET VALUES
template <typename Type>
Type Matrix<Type>::getValue(int x, int y) {
    if (!checkBounds(x,y))
    {
        std::cout<<x << " , "<<y <<" is not a bound coordinate \n \n";
        return 0;
    }
    
        return WidthLayer[Index(x,y)];
    
}
template <typename Type>
Type Matrix<Type>::getValue(int i) {
    if (!checkBounds(i))
    {
        std::cout<<i <<" is not a bound index \n \n";
        return 0;
    }
    
        return WidthLayer[i];
    
}
/////////////////////////////// END GET VALUES

/////////////////////////////// THE SET VALUES

template <typename Type>
void Matrix<Type>::setValue(int x , int y, Type input) {
    if (!checkBounds(x,y))
    {
        std::cout<<x << " , "<<y <<" is not a bound coordinate \n \n";
        return ;
    }

 WidthLayer[Index(x,y)] = input;
}

template <typename Type>
void Matrix<Type>::setValue(int i, Type input) {
    if (!checkBounds(i))
    {
        std::cout<< i  <<" is not a bound index \n \n";
        return ;
    }

 WidthLayer[i] = input;
}
/////////////////////////////// END SET VALUES

/////////////////////////////// THE GET PTR

template <typename Type>
Type* Matrix<Type>::getPointer(int x, int y)
{
        if (!checkBounds(x,y))
    {
        std::cout<<x << " , "<<y <<" is not a bound coordinate \n \n";
        return 0;
    }

    return &WidthLayer[Index(x,y)];
}

template <typename Type>
Type* Matrix<Type>::getPointer(int i)
{
    if (!checkBounds(i))
    {
        std::cout<< i  <<" is not a bound index \n \n";
        return 0 ;
    }

    return &WidthLayer[i];
}

template <typename Type>

Matrix<Type>::~Matrix()
{

    
    delete[] WidthLayer;
}


