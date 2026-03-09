#include "ui_state.h"
#include "create_maze.h"

uiState state = {
    .algoSelected = KRUSKALS,
    .menuExpanded = false,
    .menuVisible = true,
    .redrawMaze = false,
    .export = false,
    .fileName = {0},
    .fileNameLen = 0
};