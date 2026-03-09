#ifndef H_UI_UI_STATE
#define H_UI_UI_STATE

#include <stdbool.h>

typedef struct  {
    int algoSelected;
    bool menuVisible;
    bool menuExpanded;
    bool redrawMaze;
    bool export;
    char fileName[256];
    int fileNameLen;
} uiState;

extern uiState state;

#endif