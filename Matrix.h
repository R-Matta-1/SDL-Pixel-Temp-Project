#include<iostream>


template <typename Type>
class Matrix
{
private: // I don't think widthLayer should be private because of Get Pointer but tbh i have no idea how that would work
public:
int width, height; 
Type** WidthLayer ;

    Matrix(int width, int height);
    Type getValue(int x, int y);
    void setValue(int x, int y, Type input);
    Type* getPointer(int x, int y);
    void clearMatrix();
    bool checkBounds(int x, int y);
    ~Matrix();
};


template <typename Type>

Matrix<Type>::Matrix(int width, int height) 
    : width(width), height(height)
{
   WidthLayer = new Type* [width];

for (int i = 0; i < width; i++)
    {
        WidthLayer[i] = new Type [height];
    }


}

template <typename Type>
bool Matrix<Type>::checkBounds(int x, int y) {

    return (x >=0 && y >=0 && x < width && y < height );
}


template <typename Type>
void Matrix<Type>::clearMatrix() {

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            WidthLayer[x][y] = (Type)0;
        }
        
    }
    

}

template <typename Type>
Type Matrix<Type>::getValue(int x, int y) {
    if (!checkBounds(x,y))
    {
        std::cout<<x << " , "<<y <<" is not a bound coordinate \n \n";
        return 0;
    }
    
        return WidthLayer[x][y];
    
}


template <typename Type>

void Matrix<Type>::setValue(int x , int y, Type input) {
    if (!checkBounds(x,y))
    {
        std::cout<<x << " , "<<y <<" is not a bound coordinate \n \n";
        return ;
    }

 WidthLayer[x][y] = input;

}

template <typename Type>
Type* Matrix<Type>::getPointer(int x, int y)
{
        if (!checkBounds(x,y))
    {
        std::cout<<x << " , "<<y <<" is not a bound coordinate \n \n";
        return 0;
    }

    return &WidthLayer[x][y];
}

template <typename Type>

Matrix<Type>::~Matrix()
{
    for (int i = 0; i < width; i++)
    {
        delete[] WidthLayer[i];
    }
    
    delete[] WidthLayer;
}


