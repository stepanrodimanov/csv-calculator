#ifndef TABLE_H
#define TABLE_H

#include "cell.h"

typedef struct Table {
    char** headers;
    int* row_numbers;
    int col_count;
    int row_count;
    Cell** cells;
} Table;

#endif
