#ifndef CELL_H
#define CELL_H

typedef struct Cell {
    char* raw;
    int is_formula;
    int value;
    int visited;
    int error;
} Cell;

#endif
