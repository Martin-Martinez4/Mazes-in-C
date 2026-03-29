#include "ui_state.h"
#include "create_maze.h"

uiState state = {
    .cell_width = 16,
    .cell_height = 16,
    .border_thickness = 1,
    .room_saturation = 0.2f,
    .num_algos = 1,
    .prune_aggressiveness = 0,
    .hybrid_options = {BACKTRACKING,BACKTRACKING,BACKTRACKING,BACKTRACKING,BACKTRACKING},

    .algoSelected = KRUSKALS,
    .menuExpanded = false,
    .menuVisible = true,
    .redrawMaze = false,
    .export = false,
    .fileName = {0},
    .fileNameLen = 0,
    .upload = false,
    .uploadFileName = {0},
    .uploadFileNameLen = 0,
    .mCheck = nk_false
};