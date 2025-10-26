#include "draw_cells.h"
#include <stdlib.h>
#include <stdio.h>


int coordsMatrixToArray(int x, int y, int columns){
  return x * columns + y;
}

// pass by value is good enough for now
MazeStats createMazeStats(int canvasWidth, int canvasHeight, int cellWidth, int cellHeight, int borderWidth){
    // typedef struct MazeStats{
    //     int canvasWidth;
    //     int canvasHeight;
    //     int totalCellWidth;
    //     int totalCellHeight;
    //     int rows;
    //     int columns;

    // } MazeStats;

    int totalCellHeight = cellHeight + borderWidth;
    int totalCellWidth = cellWidth + borderWidth;

    // printf("totalCellHeight : %d\n", totalCellHeight);
    // printf("totalCellWidth : %d\n", totalCellWidth);
    // printf("canvasWidth : %d\n", canvasWidth);


    struct MazeStats m = {
        .canvasWidth = canvasWidth,
        .canvasHeight = canvasHeight,
        .totalCellHeight = totalCellHeight,
        .totalCellWidth = totalCellWidth,
        .rows = (int)((canvasHeight - borderWidth) / totalCellHeight),
        .columns = (int)((canvasWidth - borderWidth) / totalCellWidth),
        .borderWidth = borderWidth
    };

    return m;
}


int buildCellsArray(SDL_FRect *rects, size_t length, MazeStats mazeStats){
    int borderWidth = mazeStats.borderWidth;
    int totalCellHeight = mazeStats.totalCellHeight;
    int totalCellWidth = mazeStats.totalCellWidth;

    int verticalBorderHeight = totalCellHeight;
        
    int rows = mazeStats.rows;
    int columns = mazeStats.columns;

    // printf("Rows: %d\n", rows);
    // printf("Cols: %d\n", columns);
    // mazeStats  = createMazeStats(100, 100, 8, 8, 2);
   
    int current = 0;

    int offsetX = 0;
    int offsetY = 0;
    for(int row = 0; row < rows; row++){
        offsetY = (totalCellHeight * row);
        for(int col = 0; col < columns; col++){
            int randomNumber = rand() % 10 + 1;

            offsetX = (totalCellWidth * col);

            if(randomNumber > 2 || col == 0){

                SDL_FRect leftBorder = {
                    .x = offsetX,
                    .y = offsetY,
                    .h = verticalBorderHeight,
                    .w = borderWidth
                };
    
                rects[current] = leftBorder;
                current++;
            }

            randomNumber = rand() % 10 + 1;
            if(randomNumber > 3 || row == 0){

                SDL_FRect topBorder = {
                    .x = offsetX,
                    .y = offsetY,
                    .h = borderWidth,
                    .w = totalCellWidth
                };
    
    
                rects[current] = topBorder;
                current++;
            }

            if(col == columns-1) {
                SDL_FRect rightBorder = {
                    .x = offsetX + totalCellWidth,
                    .y = offsetY,
                    .h = verticalBorderHeight,
                    .w = borderWidth
                };


                rects[current] = rightBorder;
                current++;
            }
            if(row == rows-1){
                SDL_FRect bottomBorder = {
                    .x = offsetX ,
                    .y = offsetY + totalCellHeight,
                    .h = borderWidth,
                    .w = totalCellWidth + borderWidth
                };


                rects[current] = bottomBorder;
                current++;
            }
        }

    }

    return current;

}