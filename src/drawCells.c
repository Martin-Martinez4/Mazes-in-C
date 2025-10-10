#include <SDL3/SDL_rect.h>
#include "drawCells.h"

// return a array of pointers to rectF
// create draw takes array of pointers to rectFs converts to array of rectF struct and use that to drawRects 
// have a variable that gaurds against rebuilding the cells every frame
void drawCells(int windowHeight, int windowWidth, int cellHeight, int cellWidth, int borderWidth){
    int twiceBorderWidth = borderWidth * 2;
    int totalCellHeight = cellHeight + borderWidth * 2;
    int totalCellWidth = cellWidth + borderWidth * 2;

    int verticalBorderHeight = cellHeight + borderWidth * 2;

    int offsetX = 0;
    int offsetY = 0;
    

    // array of pointers to rect
    // border_left example
    // SDL_FRect border_left = {
        //     .x = offsetX-borderWidth-borderWidth,
        //     .y = offsetY-cellHeight-borderWidth,
        //     .h = cellHeight + borderWidth * 2,
        //     .w = borderWidth
        // };
        
        // currentRow * numCols + currentColl = 2d matrix to 1d array
        
        
        
    int rows = (int)(windowHeight / totalCellHeight);
    int columns = (int)(windowWidth / totalCellWidth);

    SDL_FRect *arr = (SDL_FRect *)malloc((rows * columns) * sizeof(SDL_FRect *));

    int offsetX = 0;
    int offsetY = 0;
    for(int row = 0; row < rows; row++){
        offsetY = totalCellHeight * row;
        for(int col = 0; col < columns; col++){
          
        }
    }

}