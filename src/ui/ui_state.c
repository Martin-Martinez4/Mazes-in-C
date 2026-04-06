#include "ui_state.h"
#include "create_maze.h"
#include "noise.h"

uiState state = {
    .cell_edges_selected  = 1,
    .cell_edges = 4,
    .cell_width = 16,
    .cell_height = 16,
    .border_thickness = 1,
    .room_saturation = 0.0f,
    .num_algos = 2,
    .prune_aggressiveness = 0,
    .hybrid_options = {BACKTRACKING,BACKTRACKING,BACKTRACKING,BACKTRACKING,BACKTRACKING},

    .generate_noise = false,
    .algoSelected = KRUSKALS,
    .noise_selected = VALUE,
    .scale = 0.0756f,
    .cx = 0,
    .cy = 0,
    .degrees = 45.0f,

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