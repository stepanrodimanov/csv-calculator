#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "table.h"

int is_number(char* s);
char* read_line(FILE* file);
void free_table(Table* table);

#endif
