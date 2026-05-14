#ifndef PARSER_H
#define PARSER_H

#include "table.h"

void parse_headers(Table* table, char* line);
void parse_data(Table* table, char* line);
Table* load_csv(char* filename);

#endif
