#include "ui_state.h"
#include "create_maze.h"

uiState state = {
    .algoSelected = KRUSKALS,
    .menuExpanded = false,
    .menuVisible = true,
    .redrawMaze = false
};