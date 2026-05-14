#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include "parser.h"
#include "utils.h"
#include "evaluator.h"

int parse_headers(Table* table, char* line) {
    if (line[0] != ',') return 1;
    
    int count = 0;
    char* token = strtok(line, ",");
    
    while (token) {
        if (token[0] == '\0' && count == 0) {
            token = strtok(NULL, ",");
            continue;
        }
        
        for (int i = 0; i < count; i++) {
            if (strcmp(table->headers[i], token) == 0) {
                fprintf(stderr, "Error: duplicate column name '%s'\n", token);
                return 1;
            }
        }
        
        table->headers = realloc(table->headers, (count + 1) * sizeof(char*));
        if (!table->headers) {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
        table->headers[count] = malloc(strlen(token) + 1);
        strcpy(table->headers[count], token);
        count++;
        
        token = strtok(NULL, ",");
    }
    
    table->col_count = count;
    return 0;
}

int parse_data(Table* table, char* line) {
    char* token = strtok(line, ",");
    if (!token) return 1;
    
    for (int i = 0; token[i]; i++) {
        if (!isdigit(token[i]) && token[i] != '-') {
            fprintf(stderr, "Error: invalid row number '%s'\n", token);
            return 1;
        }
    }
    
    char* endptr;
    errno = 0;
    long row_long = strtol(token, &endptr, 10);
    if (*endptr != '\0' || errno == ERANGE || row_long <= 0 || row_long > INT_MAX) {
        fprintf(stderr, "Error: invalid row number '%s'\n", token);
        return 1;
    }
    int row_num = (int)row_long;
    
    for (int i = 0; i < table->row_count; i++) {
        if (table->row_numbers[i] == row_num) {
            fprintf(stderr, "Error: duplicate row number %d\n", row_num);
            return 1;
        }
    }
    
    int* new_row_numbers = realloc(table->row_numbers, (table->row_count + 1) * sizeof(int));
    if (!new_row_numbers) return 1;
    table->row_numbers = new_row_numbers;
    table->row_numbers[table->row_count] = row_num;
    
    Cell** new_cells = realloc(table->cells, (table->row_count + 1) * sizeof(Cell*));
    if (!new_cells) return 1;
    table->cells = new_cells;
    
    table->cells[table->row_count] = calloc(table->col_count, sizeof(Cell));
    if (!table->cells[table->row_count]) return 1;
    
    int col = 0;
    while ((token = strtok(NULL, ",")) != NULL) {
        if (col >= table->col_count) {
            fprintf(stderr, "Error: too many columns in row %d\n", row_num);
            return 1;
        }
        
        Cell* cell = &table->cells[table->row_count][col];
        cell->raw = malloc(strlen(token) + 1);
        if (cell->raw) {
            strcpy(cell->raw, token);
            if (token[0] == '=') {
                cell->is_formula = 1;
                cell->value = 0;
            } else {
                char* end;
                errno = 0;
                long num = strtol(token, &end, 10);
                if (*end != '\0' || errno == ERANGE || num < INT_MIN || num > INT_MAX) {
                    cell->error = 1;
                } else {
                    cell->value = (int)num;
                }
            }
        }
        col++;
    }
    
    if (col != table->col_count) {
        fprintf(stderr, "Error: not enough columns in row %d (got %d, expected %d)\n", 
                row_num, col, table->col_count);
        return 1;
    }
    
    table->row_count++;
    return 0;
}

Table* load_csv(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }
    
    Table* table = calloc(1, sizeof(Table));
    if (!table) {
        fprintf(stderr, "Error: memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    char* line = read_line(file);
    if (!line) {
        fprintf(stderr, "Error: empty file\n");
        free_table(table);
        fclose(file);
        return NULL;
    }
    
    if (parse_headers(table, line) != 0) {
        fprintf(stderr, "Error: invalid headers\n");
        free(line);
        free_table(table);
        fclose(file);
        return NULL;
    }
    free(line);
    
    if (table->col_count == 0) {
        fprintf(stderr, "Error: no columns found\n");
        free_table(table);
        fclose(file);
        return NULL;
    }
    
    while ((line = read_line(file)) != NULL) {
        if (parse_data(table, line) != 0) {
            fprintf(stderr, "Error: invalid data row, aborting\n");
            free(line);
            free_table(table);
            fclose(file);
            return NULL;
        }
        free(line);
    }
    
    fclose(file);
    return table;
}
