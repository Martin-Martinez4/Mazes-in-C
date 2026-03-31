#include "kruskals.h"

#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "grid_utils.h"
#include "rooms.h"

// Kruskals is applied at the end of the hybrid maze so this does nothing
void kruskals_region(Cell* cells, int rows, int cols, MazeState* maze_state) {}