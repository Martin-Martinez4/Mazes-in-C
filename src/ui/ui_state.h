#ifndef H_UI_UI_STATE
#define H_UI_UI_STATE

#include <stdbool.h>
#include <nuklear.h>

typedef struct  {
    bool menuVisible;
    bool menuExpanded;

    int cell_width;
    int cell_height;
    int border_thickness;
    float room_saturation;
    int num_algos;
    int prune_aggressiveness;

    int hybrid_options[5];

    int algoSelected;

    bool generate_noise;
    int noise_selected;
    float scale;
    int cx;
    int cy;
    float degrees;

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