#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "drawCells.h"

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 506;

const int CELL_HEIGHT = 40;
const int CELL_WIDTH = 40;
const int BORDER_WIDTH = 2;



int main(int argc, char* argv[]) {

    printf("RUNNING\n");

    SDL_Window *window;                    // Declare a pointer
    bool done = false;

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL3

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL3 window",                  // window title
        WINDOW_WIDTH,                               // width, in pixels
        WINDOW_HEIGHT,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );

    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if(renderer == NULL){
        printf("Renderer could not be created! SDL_Error: ");
        return 1;
    }

    MazeStats mazeStats = createMazeStats((int)(WINDOW_WIDTH/2), (int)(WINDOW_HEIGHT/2), 25, 25, 4);
    
    // only holds the borders
    size_t rectsSize = ((3 * mazeStats.rows * mazeStats.columns + (mazeStats.rows + mazeStats.columns)) - (mazeStats.rows * mazeStats.columns)) * sizeof(SDL_FRect); 
    SDL_FRect *rects = (SDL_FRect *)malloc(rectsSize);
    int lenRects = rectsSize / sizeof(SDL_FRect);

    // printf("size is: %d\n", (int)lenRects);
    // printf("rows is: %d\n", mazeStats.rows);
    // printf("columns is: %d\n", mazeStats.columns);

   int cellsToDraw =  buildCellsArray(rects, lenRects, mazeStats);

    SDL_FRect background = {
        .x = 0,
        .y = 0,
        .h = mazeStats.canvasHeight,
        .w = mazeStats.canvasWidth
    };


    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        // Do game logic, present a frame, etc.

        // fill with black background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

         SDL_SetRenderDrawColor(renderer, 250, 249, 246, 255);
        int status = SDL_RenderFillRect(renderer, &background);
        
        SDL_SetRenderDrawColor(renderer, 50, 50, 246, 255);
        SDL_RenderFillRects(renderer, rects, cellsToDraw);



        SDL_RenderPresent(renderer);

    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}