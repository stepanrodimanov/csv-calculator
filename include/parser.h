#ifndef PARSER_H
#define PARSER_H

#include "table.h"

int parse_headers(Table* table, char* line);
int parse_data(Table* table, char* line);
Table* load_csv(char* filename);

#endif
