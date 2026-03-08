#ifndef H_UI_UI_STATE
#define H_UI_UI_STATE

#include <stdbool.h>

typedef struct  {
    int algoSelected;
    bool menuVisible;
    bool menuExpanded;
    bool redrawMaze;
} uiState;

extern uiState state;

#endif