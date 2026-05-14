#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "utils.h"
#include "evaluator.h"
#include <ctype.h>

void parse_headers(Table* table, char* line) {
    int count = 0;
    char* token = strtok(line, ",");
    while (token) {
        table->headers = realloc(table->headers, (count + 1) * sizeof(char*));
        table->headers[count] = malloc(strlen(token) + 1);
        strcpy(table->headers[count], token);
        count++;
        token = strtok(NULL, ",");
    }
    table->col_count = count;
}

void parse_data(Table* table, char* line) {
    char* token = strtok(line, ",");
    if (!token) return;
    
    for (int i = 0; token[i]; i++) {
        if (!isdigit(token[i]) && token[i] != '-') {
            fprintf(stderr, "Error: invalid row number '%s', skipping row\n", token);
            return;
        }
    }

    int row_num = atoi(token);
    if (row_num <= 0) {
        fprintf(stderr, "Warning: invalid row number %d\n", row_num);
        return;
    }

    if (token) {
        table->row_numbers = realloc(table->row_numbers, (table->row_count + 1) * sizeof(int));
        if (!table->row_numbers) {
            fprintf(stderr, "Error: memory allocation failed\n");
            return;
        }
        table->row_numbers[table->row_count] = row_num;
    }

    table->cells = realloc(table->cells, (table->row_count + 1) * sizeof(Cell*));
    if (!table->cells) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return;
    }

    table->cells[table->row_count] = calloc(table->col_count, sizeof(Cell));
    if (!table->cells[table->row_count]) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return;
    }

    int col = 0;
    while ((token = strtok(NULL, ",")) != NULL) {
         if (col >= table->col_count) {
            fprintf(stderr, "Warning: too many columns in row %d, skipping excess\n", row_num);
            break;
        }
        Cell* cell = &table->cells[table->row_count][col];
        cell->raw = malloc((strlen(token) + 1) * sizeof(char));
        if (cell->raw) {
            strcpy(cell->raw, token);
            if (token[0] == '=') {
                cell->is_formula = 1;
                cell->value = 0;
            } else if (is_number(token)) {
                cell->value = atoi(token);
            } else {
                cell->error = 1;
            }
        }
        col++;
    }
    table->row_count++;
    
}

Table* load_csv(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file.\n");
        return NULL;
    }
    
    Table* table = calloc(1, sizeof(Table));
    
    char* line = read_line(file);
    if (line) {
        parse_headers(table, line);
        free(line); 
    }
    while ((line = read_line(file)) != NULL) {
        parse_data(table, line);
        free(line); 
    }
    
    fclose(file);
    return table;
}
