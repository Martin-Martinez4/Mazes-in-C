#include "load.h"
#include "stdio.h"
#include "cell.h"
#include "stdlib.h"
#include "grid_utils.h"

Cell* loadMaze(MazeStats* mazeStats, int* cellsCreated, char* filePath) {

    FILE* file = fopen(filePath, "rb");
    if (!file) {
        perror("Could not open file");
        return NULL;
    }

    int l;

    int magicNum[4] = {109, 97, 122, 101}; // "maze"

    for (uint8_t i = 0; i < 4; i++) {
        if (fread(&l, sizeof(int), 1, file) != 1) {
            perror("Failed to read magic number");
            fclose(file);
            return NULL;
        }

        if (l != magicNum[i]) {
            perror("File type check failed");
            fclose(file);
            return NULL;
        }
    }

    int columns;
    int rows;

    if (fread(&columns, sizeof(int), 1, file) != 1 ||
        fread(&rows, sizeof(int), 1, file) != 1) {

        perror("Failed to read maze dimensions");
        fclose(file);
        return NULL;
    }

    printf("columns: %d; rows: %d\n", columns, rows);

    mazeStats->rows = rows;
    mazeStats->columns = columns;

    int totalCells = rows * columns;

    Cell* cells = malloc(sizeof(Cell) * totalCells);
    if (!cells) {
        perror("malloc failed");
        fclose(file);
        return NULL;
    }

    int r, c;

    for (size_t i = 0; i < totalCells; i++) {

        if (fread(&l, sizeof(uint8_t), 1, file) != 1) {
            perror("Unexpected end of file");
            free(cells);
            fclose(file);
            return NULL;
        }

        array_coords_to_matrix_coords(i, columns, &r, &c);

        cells[i] = (Cell){
            .column = c,
            .row = r,
            .walls = l
        };
    }

    *cellsCreated = totalCells;

    fclose(file);

    return cells;
}
