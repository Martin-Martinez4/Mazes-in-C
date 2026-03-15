#ifndef H_UI_UI_STATE
#define H_UI_UI_STATE

#include <stdbool.h>
#include <nuklear.h>

typedef struct  {
    int algoSelected;
    bool menuVisible;
    bool menuExpanded;
    bool redrawMaze;
    bool export;
    char fileName[256];
    int fileNameLen;
    bool upload;
    char uploadFileName[256];
    int uploadFileNameLen;
    nk_bool mCheck;
} uiState;

extern uiState state;

#endif