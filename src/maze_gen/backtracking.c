#include <string.h>

#include "backtracking.h"
#include "grid_utils.h"
#include "rand_utils.h"

void shuffleArray(void* array, int length, size_t element_size) {
  // just to get the pointer ot the first thing
  char* arr = (char*) array;

  for (int i = length - 1; i >= 1; i--) {
    int j = rand() % (i + 1);

    char* temp = malloc(element_size);

    // swap arr[i] and arr[j]
    memcpy(temp, arr + i * element_size, element_size);
    memcpy(arr + i * element_size, arr + j * element_size, element_size);
    memcpy(arr + j * element_size, temp, element_size);
  }
}

void backtrack(Cell* cells, bool* visited, int row, int column, int rows, int columns) {

  int array_coords = matrix_coords_to_array_coords(row, column, columns);

  if (visited[array_coords]) {
    return;
  }
  visited[array_coords] = true;

  int sides[4] = {TOP, LEFT, BOTTOM, RIGHT};
  shuffleArray(sides, 4, sizeof(int));

  for (int i = 0; i < 4; i++) {
    int neigh_index = 0;

    switch (sides[i]) {

    case TOP:
      if (row == 0) {
        continue;
      }

      neigh_index = matrix_coords_to_array_coords(row - 1, column, columns);
      if (!visited[neigh_index]) {
        cells[array_coords].walls &= ~TOP;
        cells[neigh_index].walls &= ~BOTTOM;
        backtrack(cells, visited, row - 1, column, rows, columns);
      }
      break;

    case LEFT:
      if (column == 0) {
        continue;
      }

      neigh_index = matrix_coords_to_array_coords(row, column - 1, columns);
      if (!visited[neigh_index]) {
        cells[array_coords].walls &= ~LEFT;
        cells[neigh_index].walls &= ~RIGHT;
        backtrack(cells, visited, row, column - 1, rows, columns);
      }
      break;

    case BOTTOM:
      if (row == rows - 1) {
        continue;
      }

      neigh_index = matrix_coords_to_array_coords(row + 1, column, columns);
      if (!visited[neigh_index]) {

        cells[array_coords].walls &= ~BOTTOM;
        cells[neigh_index].walls &= ~TOP;
        backtrack(cells, visited, row + 1, column, rows, columns);
      }
      break;

    case RIGHT:
      if (column == columns - 1) {
        continue;
      }

      neigh_index = matrix_coords_to_array_coords(row, column + 1, columns);
      if (!visited[neigh_index]) {

        cells[array_coords].walls &= ~RIGHT;
        cells[neigh_index].walls &= ~LEFT;
        backtrack(cells, visited, row, column + 1, rows, columns);
      }
      break;

    default:
      break;
    }
  }
}

// Idea: Make reusable and pass in a function to call to handle the rooms
void setUpCellsbt(Cell* cells, Rooms* rooms, int rows, int columns) {
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      cells[matrix_coords_to_array_coords(row, col, columns)] = create_walled_cell(row, col);
    }
  }

  if (rooms == NULL) {
    return;
  }

  for (int i = 0; i < rooms->length; i++) {
    Room room = rooms->data[i];
    int set   = matrix_coords_to_array_coords(room.aabb.y, room.aabb.x, columns);

    int x      = room.aabb.x;
    int y      = room.aabb.y;
    int width  = room.aabb.width;
    int height = room.aabb.height;

    for (int row = y; row < (y + height); row++) {
      for (int col = x; col < (x + width); col++) {
        int walls = 0;
        if (row == y)
          walls |= TOP;
        if (row == y + height - 1)
          walls |= BOTTOM;
        if (col == x)
          walls |= LEFT;
        if (col == x + width - 1)
          walls |= RIGHT;
        cells[matrix_coords_to_array_coords(row, col, columns)].walls = walls;
      }
    }
  }
}

// To deal with edge cases of high room saturation.
void ensure_all_rooms_connected(Cell* cells, bool* visited, Rooms* rooms, int rows, int columns) {
  if (rooms == NULL)
    return;

  for (int i = 0; i < rooms->length; i++) {
    Room room  = rooms->data[i];
    int x      = room.aabb.x;
    int y      = room.aabb.y;
    int width  = room.aabb.width;
    int height = room.aabb.height;

    bool connected = false;

    // Try to open a doorway on any wall touching the maze
    for (int row = y; row < y + height && !connected; row++) {
      for (int col = x; col < x + width && !connected; col++) {
        // Only consider wall cells (edges of room)
        if (row != y && row != y + height - 1 && col != x && col != x + width - 1)
          continue;

        int array_coords = matrix_coords_to_array_coords(row, col, columns);

        // Check neighbors for maze cells
        int neigh[4][2] = {{row - 1, col}, {row + 1, col}, {row, col - 1}, {row, col + 1}};

        for (int n = 0; n < 4; n++) {
          int nr = neigh[n][0];
          int nc = neigh[n][1];

          if (nr >= 0 && nr < rows && nc >= 0 && nc < columns) {
            int neigh_index = matrix_coords_to_array_coords(nr, nc, columns);
            if (!visited[neigh_index]) {
              // open doorway
              visited[array_coords] = false;
              connected             = true;
              break;
            }
          }
        }
      }
    }

    // Fallback if room is completely sealed
    if (!connected) {
      int center_row = y + height / 2;
      int center_col = x + width / 2;

      int nearest_row = -1, nearest_col = -1;
      int best_dist = rows * columns;

      for (int row2 = 0; row2 < rows; row2++) {
        for (int col2 = 0; col2 < columns; col2++) {
          int idx = matrix_coords_to_array_coords(row2, col2, columns);
          if (!visited[idx]) { // maze cell
            int dx = 0, dy = 0;
            if (col2 < x)
              dx = x - col2;
            else if (col2 >= x + width)
              dx = col2 - (x + width - 1);

            if (row2 < y)
              dy = y - row2;
            else if (row2 >= y + height)
              dy = row2 - (y + height - 1);

            int dist = dx + dy;
            if (dist < best_dist) {
              best_dist   = dist;
              nearest_row = row2;
              nearest_col = col2;
            }
          }
        }
      }

      // Carve Manhattan corridor from room center to nearest maze cell
      if (nearest_row != -1 && nearest_col != -1) {
        int r = center_row;
        int c = center_col;

        while (r != nearest_row || c != nearest_col) {
          int idx      = matrix_coords_to_array_coords(r, c, columns);
          visited[idx] = false;

          if (r < nearest_row)
            r++;
          else if (r > nearest_row)
            r--;
          else if (c < nearest_col)
            c++;
          else if (c > nearest_col)
            c--;
        }
      }
    }
  }
}

Cell* backtrackingCreateMaze(MazeStats* mazeStates, Rooms* rooms) {
  int columns = mazeStates->columns;
  int rows    = mazeStates->rows;

  bool* visited = malloc(sizeof(bool) * columns * rows);

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      visited[matrix_coords_to_array_coords(row, col, columns)] = false;
    }
  }

  if (rooms != NULL) {

    for (int i = 0; i < rooms->length; i++) {
      Room room = rooms->data[i];
      int set   = matrix_coords_to_array_coords(room.aabb.y, room.aabb.x, columns);

      int x      = room.aabb.x;
      int y      = room.aabb.y;
      int width  = room.aabb.width;
      int height = room.aabb.height;

      for (int row = y; row < (y + height); row++) {
        for (int col = x; col < (x + width); col++) {
          int walls = 0;
          if (row == y || row == y + height - 1 || col == x || col == x + width - 1) {
            continue;
          } else {

            visited[matrix_coords_to_array_coords(row, col, columns)] = true;
          }
        }
      }
    }
  }

  Cell* cells = malloc(sizeof(Cell) * columns * rows);

  setUpCellsbt(cells, rooms, rows, columns);

  typedef struct {
    int r, c;
  } CellPos;

  int freeCount      = 0;
  CellPos* freeCells = malloc(sizeof(CellPos) * rows * columns);

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      if (!visited[matrix_coords_to_array_coords(row, col, columns)]) {
        freeCells[freeCount++] = (CellPos){row, col};
      }
    }
  }

  if (freeCount > 0) {
    CellPos start = freeCells[rand_int(0, freeCount - 1)];
    backtrack(cells, visited, start.r, start.c, rows, columns);
    ensure_all_rooms_connected(cells, visited, rooms, rows, columns);
  }

  // check for cells that are not visited
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      int index = matrix_coords_to_array_coords(row, col, columns);
      if (!visited[index]) {
        backtrack(cells, visited, row, col, rows, columns);
      }
    }
  }

  free(freeCells);
  free(visited);

  return cells;
}